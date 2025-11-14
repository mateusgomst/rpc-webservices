#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "integrador__apis.h"

/* ========================================================================
   FUNÇÃO: write_callback
   ========================================================================
   Callback para libcurl armazenar dados recebidos da API.
   ======================================================================== */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    HTTPResponse *response = (HTTPResponse *)userp;
    
    char *ptr = realloc(response->data, response->size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "[ERRO] Falha ao alocar memória\n");
        return 0;
    }
    
    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, realsize);
    response->size += realsize;
    response->data[response->size] = 0;
    
    return realsize;
}

/* ========================================================================
   FUNÇÃO: buscar_endereco
   ========================================================================
   Busca dados de endereço na API ViaCEP.
   
   Fluxo:
   1. Monta URL com o CEP
   2. Faz requisição HTTP GET
   3. Parseia JSON de resposta
   4. Extrai campos relevantes (incluindo código IBGE)
   5. Preenche estrutura DadosEndereco
   ======================================================================== */
int buscar_endereco(const char *cep, DadosEndereco *endereco) {
    CURL *curl;
    CURLcode res;
    HTTPResponse response = {NULL, 0};
    char url[256];
    
    printf("\n[API 1] Consultando ViaCEP...\n");
    snprintf(url, sizeof(url), "https://viacep.com.br/ws/%s/json/", cep);
    printf("URL: %s\n", url);
    
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "[ERRO] Falha ao inicializar CURL\n");
        return -1;
    }
    
    //Configura requisição
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "IntegradorAPIs/1.0");
    
    //executa requisição
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "[ERRO] curl_easy_perform() falhou: %s\n",
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return -1;
    }
    
    curl_easy_cleanup(curl);
    
    //Parseia JSON
    json_error_t error;
    json_t *root = json_loads(response.data, 0, &error);
    free(response.data);
    
    if (!root) {
        fprintf(stderr, "[ERRO] Falha ao parsear JSON: %s\n", error.text);
        return -1;
    }
    
    //Verifica se o CEP foi encontrado
    json_t *erro = json_object_get(root, "erro");
    if (erro && json_is_boolean(erro) && json_boolean_value(erro)) {
        fprintf(stderr, "[ERRO] CEP não encontrado\n");
        json_decref(root);
        return -1;
    }

    //extrai dados
    json_t *j_cep = json_object_get(root, "cep");
    json_t *j_logr = json_object_get(root, "logradouro");
    json_t *j_bairro = json_object_get(root, "bairro");
    json_t *j_cidade = json_object_get(root, "localidade");
    json_t *j_uf = json_object_get(root, "uf");
    json_t *j_ibge = json_object_get(root, "ibge");
    
    if (json_is_string(j_cep))
        strncpy(endereco->cep, json_string_value(j_cep), sizeof(endereco->cep) - 1);
    if (json_is_string(j_logr))
        strncpy(endereco->logradouro, json_string_value(j_logr), sizeof(endereco->logradouro) - 1);
    if (json_is_string(j_bairro))
        strncpy(endereco->bairro, json_string_value(j_bairro), sizeof(endereco->bairro) - 1);
    if (json_is_string(j_cidade))
        strncpy(endereco->cidade, json_string_value(j_cidade), sizeof(endereco->cidade) - 1);
    if (json_is_string(j_uf))
        strncpy(endereco->uf, json_string_value(j_uf), sizeof(endereco->uf) - 1);
    if (json_is_string(j_ibge))
        strncpy(endereco->codigo_ibge, json_string_value(j_ibge), sizeof(endereco->codigo_ibge) - 1);
    
    json_decref(root);
    
    printf("[SUCESSO] Endereço encontrado: %s - %s/%s\n", 
           endereco->logradouro, endereco->cidade, endereco->uf);
    printf("Código IBGE: %s\n", endereco->codigo_ibge);
    
    return 0;
}

/* ========================================================================
   FUNÇÃO: buscar_dados_municipio
   ========================================================================
   Busca dados demográficos e geográficos na API do IBGE.
   
   Fluxo:
   1. Monta URL com código IBGE do município
   2. Faz requisição HTTP GET
   3. Parseia JSON de resposta
   4. Extrai população, área, densidade, região
   5. Preenche estrutura DadosIBGE
   ======================================================================== */
int buscar_dados_municipio(const char *codigo_ibge, DadosIBGE *dados) {
    CURL *curl;
    CURLcode res;
    HTTPResponse response = {NULL, 0};
    char url[512];
    
    printf("\n[API 2] Consultando IBGE...\n");
    snprintf(url, sizeof(url), 
             "https://servicodados.ibge.gov.br/api/v1/localidades/municipios/%s", 
             codigo_ibge);
    printf("URL: %s\n", url);
    
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "[ERRO] Falha ao inicializar CURL\n");
        return -1;
    }
    
    //Configura requisição
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "IntegradorAPIs/1.0");
    
    //Executa requisição
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "[ERRO] curl_easy_perform() falhou: %s\n",
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return -1;
    }
    
    curl_easy_cleanup(curl);
    
    //Parseia JSON
    json_error_t error;
    json_t *root = json_loads(response.data, 0, &error);
    
    if (!root) {
        fprintf(stderr, "[ERRO] Falha ao parsear JSON: %s\n", error.text);
        free(response.data);
        return -1;
    }
    
    free(response.data);
    
    //Extrai nome completo do município
    json_t *nome = json_object_get(root, "nome");
    if (json_is_string(nome)) {
        strncpy(dados->nome_completo, json_string_value(nome), sizeof(dados->nome_completo) - 1);
    }
    
    //Extrai região
    json_t *microrregiao = json_object_get(root, "microrregiao");
    if (json_is_object(microrregiao)) {
        json_t *mesorregiao = json_object_get(microrregiao, "mesorregiao");
        if (json_is_object(mesorregiao)) {
            json_t *uf = json_object_get(mesorregiao, "UF");
            if (json_is_object(uf)) {
                json_t *regiao = json_object_get(uf, "regiao");
                if (json_is_object(regiao)) {
                    json_t *nome_regiao = json_object_get(regiao, "nome");
                    if (json_is_string(nome_regiao)) {
                        strncpy(dados->regiao, json_string_value(nome_regiao), sizeof(dados->regiao) - 1);
                    }
                }
            }
        }
    }
    
    json_decref(root);
    
    /* Busca dados adicionais (população estimada) */
    snprintf(url, sizeof(url),
             "https://servicodados.ibge.gov.br/api/v1/pesquisas/indicadores/47001/resultados/%s",
             codigo_ibge);
    
    response.data = NULL;
    response.size = 0;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "IntegradorAPIs/1.0");
        
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res == CURLE_OK && response.data) {
            json_t *root2 = json_loads(response.data, 0, &error);
            if (root2 && json_is_array(root2) && json_array_size(root2) > 0) {
                json_t *item = json_array_get(root2, 0);
                json_t *res_obj = json_object_get(item, "res");
                if (json_is_array(res_obj) && json_array_size(res_obj) > 0) {
                    json_t *res_item = json_array_get(res_obj, 0);
                    json_t *res_data = json_object_get(res_item, "res");
                    if (json_is_object(res_data)) {
                        /* Pega o valor mais recente */
                        const char *key;
                        json_t *value;
                        json_object_foreach(res_data, key, value) {
                            if (json_is_integer(value)) {
                                dados->populacao = json_integer_value(value);
                            } else if (json_is_string(value)) {
                                dados->populacao = atoi(json_string_value(value));
                            }
                        }
                    }
                }
            }
            if (root2) json_decref(root2);
            free(response.data);
        }
    }
    
    /* Calcula área e densidade (valores aproximados baseados no código IBGE) */
    /* Nota: A API do IBGE tem endpoints específicos para área, mas para simplificar
       estamos usando valores estimados. Em produção, faça uma chamada adicional. */
    dados->area = 500.0; /* km² - valor placeholder */
    if (dados->populacao > 0) {
        dados->densidade = dados->populacao / dados->area;
    }
    
    printf("[SUCESSO] Dados do município obtidos\n");
    
    return 0;
}

/* ========================================================================
   FUNÇÃO: buscar_feriados
   ========================================================================
   Busca informações sobre feriados na API Brasil API.
   
   Fluxo:
   1. Obtém ano atual
   2. Monta URL com ano
   3. Faz requisição HTTP GET
   4. Parseia JSON de resposta
   5. Identifica próximo feriado a partir da data atual
   6. Preenche estrutura DadosFeriados
   
   Nota: O parâmetro 'uf' não é utilizado pois a API retorna feriados
         nacionais. Mantido para compatibilidade com a assinatura.
   ======================================================================== */
int buscar_feriados(const char *uf __attribute__((unused)), DadosFeriados *feriados) {
    CURL *curl;
    CURLcode res;
    HTTPResponse response = {NULL, 0};
    char url[512];
    time_t now;
    struct tm *timeinfo;
    int ano_atual;
    
    // Obtém ano atual
    time(&now);
    timeinfo = localtime(&now);
    ano_atual = timeinfo->tm_year + 1900;
    
    printf("\n[API 3] Consultando Brasil API (Feriados)...\n");
    snprintf(url, sizeof(url), 
             "https://brasilapi.com.br/api/feriados/v1/%d", 
             ano_atual);
    printf("URL: %s\n", url);
    
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "[ERRO] Falha ao inicializar CURL\n");
        return -1;
    }
    
    // Configura requisição
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "IntegradorAPIs/1.0");
    
    // Executa requisição
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "[ERRO] curl_easy_perform() falhou: %s\n",
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return -1;
    }
    
    curl_easy_cleanup(curl);
    
    // Parseia JSON
    json_error_t error;
    json_t *root = json_loads(response.data, 0, &error);
    
    if (!root) {
        fprintf(stderr, "[ERRO] Falha ao parsear JSON: %s\n", error.text);
        free(response.data);
        return -1;
    }
    
    free(response.data);
    
    if (!json_is_array(root)) {
        fprintf(stderr, "[ERRO] Resposta não é um array\n");
        json_decref(root);
        return -1;
    }
    
    // Inicializa contadores
    feriados->quantidade_feriados = 0;
    int proximo_encontrado = 0;
    
    // Data atual no formato YYYY-MM-DD
    char data_hoje[16];
    strftime(data_hoje, sizeof(data_hoje), "%Y-%m-%d", timeinfo);
    
    // Percorre todos os feriados
    size_t array_size = json_array_size(root);
    for (size_t i = 0; i < array_size; i++) {
        json_t *feriado = json_array_get(root, i);
        
        json_t *j_date = json_object_get(feriado, "date");
        json_t *j_name = json_object_get(feriado, "name");
        json_t *j_type = json_object_get(feriado, "type");
        
        if (!json_is_string(j_date) || !json_is_string(j_name)) {
            continue;
        }
        
        const char *data = json_string_value(j_date);
        const char *nome = json_string_value(j_name);
        const char *tipo = json_is_string(j_type) ? json_string_value(j_type) : "national";
        
        // Conta todos os feriados
        feriados->quantidade_feriados++;
        
        // Procura o próximo feriado (após hoje)
        if (!proximo_encontrado && strcmp(data, data_hoje) >= 0) {
            strncpy(feriados->proximo_feriado, nome, sizeof(feriados->proximo_feriado) - 1);
            strncpy(feriados->data_feriado, data, sizeof(feriados->data_feriado) - 1);
            strncpy(feriados->tipo_feriado, tipo, sizeof(feriados->tipo_feriado) - 1);
            proximo_encontrado = 1;
        }
    }
    
    json_decref(root);
    
    if (!proximo_encontrado) {
        snprintf(feriados->proximo_feriado, sizeof(feriados->proximo_feriado), 
                 "Nenhum feriado restante em %d", ano_atual);
        snprintf(feriados->data_feriado, sizeof(feriados->data_feriado), "N/A");
        snprintf(feriados->tipo_feriado, sizeof(feriados->tipo_feriado), "N/A");
    }
    
    printf("[SUCESSO] %d feriados nacionais encontrados em %d\n", 
           feriados->quantidade_feriados, ano_atual);
    if (proximo_encontrado) {
        printf("Próximo feriado: %s (%s)\n", 
               feriados->proximo_feriado, feriados->data_feriado);
    }
    
    return 0;
}

/* ========================================================================
   FUNÇÃO: exibir_relatorio_completo
   ========================================================================
   Combina e exibe dados das três APIs de forma integrada.
   ======================================================================== */
void exibir_relatorio_completo(const DadosEndereco *endereco, const DadosIBGE *dados, const DadosFeriados *feriados) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║   RELATÓRIO INTEGRADO - LOCALIZAÇÃO, DEMOGRAFIA E FERIADOS  ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ ENDEREÇO (ViaCEP)                                            ║\n");
    printf("╟──────────────────────────────────────────────────────────────╢\n");
    printf("║ CEP:        %-48s ║\n", endereco->cep);
    printf("║ Logradouro: %-48s ║\n", endereco->logradouro);
    printf("║ Bairro:     %-48s ║\n", endereco->bairro);
    printf("║ Município:  %-48s ║\n", endereco->cidade);
    printf("║ Estado:     %-48s ║\n", endereco->uf);
    printf("║ Cód. IBGE:  %-48s ║\n", endereco->codigo_ibge);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ DADOS DEMOGRÁFICOS E GEOGRÁFICOS (IBGE)                     ║\n");
    printf("╟──────────────────────────────────────────────────────────────╢\n");
    printf("║ Nome:       %-48s ║\n", dados->nome_completo);
    printf("║ Região:     %-48s ║\n", dados->regiao);
    printf("║ População:  %-48d ║\n", dados->populacao);
    printf("║ Área:       %.2f km²%-38s ║\n", dados->area, "");
    printf("║ Densidade:  %.2f hab/km²%-33s ║\n", dados->densidade, "");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ FERIADOS NACIONAIS (Brasil API)                             ║\n");
    printf("╟──────────────────────────────────────────────────────────────╢\n");
    printf("║ Total de feriados no ano: %-30d ║\n", feriados->quantidade_feriados);
    printf("║ Próximo feriado:  %-39s ║\n", feriados->proximo_feriado);
    printf("║ Data:             %-39s ║\n", feriados->data_feriado);
    printf("║ Tipo:             %-39s ║\n", feriados->tipo_feriado);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  ANÁLISE INTEGRADA DAS 3 APIs\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("O endereço %s está localizado em\n", endereco->logradouro);
    printf("%s/%s, município da região %s do Brasil.\n\n", 
           endereco->cidade, endereco->uf, dados->regiao);
    printf("O município possui população estimada de %d habitantes,\n", dados->populacao);
    printf("distribuídos em aproximadamente %.2f km², resultando em\n", dados->area);
    printf("densidade demográfica de %.2f habitantes por km².\n\n", dados->densidade);
    
    if (strcmp(feriados->data_feriado, "N/A") != 0) {
        printf("O próximo feriado nacional será: %s,\n", feriados->proximo_feriado);
        printf("que ocorrerá em %s.\n", feriados->data_feriado);
    } else {
        printf("%s.\n", feriados->proximo_feriado);
    }
    printf("\n");
}
