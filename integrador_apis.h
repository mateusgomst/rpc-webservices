#ifndef INTEGRADOR_APIS_H
#define INTEGRADOR_APIS_H

#include <curl/curl.h>
#include <jansson.h>

/* Estrutura para armazenar dados do ViaCEP */
typedef struct {
    char cep[16];
    char logradouro[256];
    char bairro[128];
    char cidade[128];
    char uf[4];
    char codigo_ibge[16];
} DadosEndereco;

/* Estrutura para armazenar dados do IBGE */
typedef struct {
    char nome_completo[256];
    char regiao[64];
    int populacao;
    double area;
    double densidade;
} DadosIBGE;

/* Estrutura para armazenar dados de feriados */
typedef struct {
    int quantidade_feriados;
    char proximo_feriado[128];
    char data_feriado[16];
    char tipo_feriado[64];
} DadosFeriados;

/* Estrutura para resposta HTTP */
typedef struct {
    char *data;
    size_t size;
} HTTPResponse;

/* Funções principais */
int buscar_endereco(const char *cep, DadosEndereco *endereco);
int buscar_dados_municipio(const char *codigo_ibge, DadosIBGE *dados);
int buscar_feriados(const char *uf, DadosFeriados *feriados);
void exibir_relatorio_completo(const DadosEndereco *endereco, const DadosIBGE *dados, const DadosFeriados *feriados);

#endif
