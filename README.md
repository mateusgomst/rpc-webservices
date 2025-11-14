# Sistema de Integração de 3 APIs REST em C

## 📋 Descrição do Projeto

Construí uma aplicação que integra **três APIs públicas brasileiras** para fornecer informações completas sobre localidades através de um único CEP:

1. **ViaCEP** - Consulta informações de endereço através do CEP
2. **IBGE** - Busca dados demográficos e geográficos usando o código IBGE retornado pelo ViaCEP
3. **Brasil API** - Obtém informações sobre feriados nacionais

A aplicação **combina** esses dados para criar um relatório integrado e detalhado, fornecendo:
- ✅ Endereço completo (rua, bairro, cidade, estado)
- ✅ Dados demográficos oficiais (população, área territorial, densidade)
- ✅ Informações sobre feriados nacionais do ano atual

### 🎯 Integração Real Entre Sistemas

A chave da integração está no **código IBGE** retornado pelo ViaCEP, que é usado como parâmetro para consultar a API do IBGE. Assim:

## 🔗 APIs Utilizadas

### API 1: ViaCEP
- **URL:** https://viacep.com.br/
- **Endpoint:** `https://viacep.com.br/ws/{CEP}/json/`
- **Dados extraídos:** 
  - CEP formatado
  - Logradouro (rua/avenida)
  - Bairro
  - Cidade (localidade)
  - Estado (UF)
  - **Código IBGE** (usado para integração com API 2)

### API 2: IBGE
- **URL:** https://servicodados.ibge.gov.br/
- **Endpoints:**
  - Municípios: `https://servicodados.ibge.gov.br/api/v1/localidades/municipios/{codigo_ibge}`
  - População: `https://servicodados.ibge.gov.br/api/v1/pesquisas/indicadores/47001/resultados/{codigo_ibge}`
- **Dados extraídos:**
  - Nome completo do município
  - Região do Brasil (Norte, Nordeste, Sul, Sudeste, Centro-Oeste)
  - População estimada
  - Área territorial
  - Densidade demográfica

### API 3: Brasil API
- **URL:** https://brasilapi.com.br/
- **Endpoint:** `https://brasilapi.com.br/api/feriados/v1/{ano}`
- **Dados extraídos:**
  - Lista completa de feriados nacionais
  - Próximo feriado
  - Data e tipo do feriado

## 🔄 Fluxo de Integração

### 1. Entrada de Dados
- O usuário fornece um **CEP** válido como entrada.

### 2. Consulta à API ViaCEP
- A aplicação faz uma requisição à API ViaCEP para obter o endereço completo e o código IBGE referente ao CEP informado.
- Exemplo de requisição: `GET https://viacep.com.br/ws/01310100/json/`

### 3. Consulta à API IBGE
- Com o código IBGE em mãos, a aplicação consulta a API IBGE para obter dados demográficos e geográficos do município.
- Exemplo de requisição: `GET https://servicodados.ibge.gov.br/api/v1/localidades/municipios/3550308`

### 4. Consulta à Brasil API
- A aplicação consulta a Brasil API para obter informações sobre feriados nacionais no ano atual.
- Exemplo de requisição: `GET https://brasilapi.com.br/api/feriados/v1/2023`

### 5. Geração do Relatório
- Com os dados obtidos das três APIs, a aplicação gera um relatório completo contendo:
  - Endereço formatado
  - Dados demográficos (população, área, densidade)
  - Lista de feriados nacionais

## 🚀 Como Executar

### Pré-requisitos
- Ter o **gcc** instalado para compilação.
- Bibliotecas **libcurl4-openssl-dev** e **libjansson-dev** devem estar instaladas.

### Instalação de Dependências
```bash
sudo apt-get update
sudo apt-get install gcc libcurl4-openssl-dev libjansson-dev
```

### Compilação
```bash
make
```

### Execução
```bash
./integrador_apis <CEP>
```
Exemplo:
```bash
./integrador_apis 01310100
```

## 📂 Estrutura do Código
```
integrador_apis.h    → Definições de estruturas e protótipos
integrador_apis.c    → Implementação das funções de API
main.c               → Programa principal
Makefile             → Automação da compilação
```

## 📝 Considerações Finais

- A aplicação foi desenvolvida como um projeto acadêmico para demonstrar a integração de múltiplas APIs REST.
- Todas as APIs utilizadas são públicas e não requerem autenticação ou chaves de acesso.
- O foco do projeto é a obtenção e combinação de dados de diferentes fontes para geração de relatórios informativos.
