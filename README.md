# Sistema de Integração de APIs REST em C

## Descrição

Aplicação que combina duas APIs públicas brasileiras totalmente gratuitas:
- **ViaCEP**: Converte CEP em endereço completo (+ código IBGE)
- **IBGE**: Fornece dados demográficos e geográficos do município

**Vantagens:**
- ✅ Nenhuma chave de API necessária
- ✅ APIs 100% públicas e gratuitas
- ✅ Dados oficiais brasileiros
- ✅ Sem limites de requisições restritivos

## Instalação de Dependências

```bash
sudo apt-get update
sudo apt-get install gcc libcurl4-openssl-dev libjansson-dev
```

## Compilação

```bash
make
```

## Configuração

Nenhuma configuração é necessária. As APIs são públicas e não exigem chave de acesso.

## Uso

```bash
./integrador_apis <CEP>
```

Exemplo:
```bash
./integrador_apis 01310100
```

## Explicação da Integração

### Fluxo de Dados

1. **Entrada**: Usuário fornece CEP (ex: 01310100)

2. **API 1 - ViaCEP**:
   - Requisição: `GET https://viacep.com.br/ws/01310100/json/`
   - Resposta: JSON com endereço completo
   - Dados extraídos: logradouro, bairro, cidade, UF

3. **API 2 - IBGE**:
   - Usa a **cidade** obtida na API 1
   - Requisição: `GET https://servicodados.ibge.gov.br/api/v2/malhas/`
   - Resposta: JSON com dados demográficos e geográficos
   - Dados extraídos: população, área, densidade demográfica

4. **Combinação**: 
   - O programa combina os dados de ambas as APIs
   - Exibe relatório integrado mostrando endereço + dados demográficos

### Suficiência dos Dados

Os dados das APIs são **suficientes** porque:

- **ViaCEP fornece**: Identificação precisa da localização (cidade/estado) e código IBGE
- **IBGE fornece**: Dados demográficos e geográficos oficiais do município
- **Ligação**: O campo `localidade` (cidade) do ViaCEP é usado para obter dados na API do IBGE

Assim, a **saída de uma API alimenta a entrada da outra**, criando integração real.

## Estrutura do Código

```
integrador_apis.h    → Definições de estruturas e protótipos
integrador_apis.c    → Implementação das funções de API
main.c               → Programa principal
Makefile             → Automação da compilação
```
