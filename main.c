#include <stdio.h>
#include <stdlib.h>
#include "integrador_apis.h"

int main(int argc, char *argv[]) {
    DadosEndereco endereco = {0};
    DadosIBGE dados_ibge = {0};
    const char *cep;
    
    //valida argumentos
    if (argc < 2) {
        fprintf(stderr, "\nUso: %s <CEP>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 01310100\n\n", argv[0]);
        fprintf(stderr, "Exemplos de CEPs para testar:\n");
        fprintf(stderr, "  01310100 - São Paulo/SP (Av. Paulista)\n");
        fprintf(stderr, "  20040020 - Rio de Janeiro/RJ (Centro)\n");
        fprintf(stderr, "  30130100 - Belo Horizonte/MG (Centro)\n");
        fprintf(stderr, "  40020000 - Salvador/BA (Centro)\n");
        fprintf(stderr, "  88015100 - Florianópolis/SC (Centro)\n\n");
        return 1;
    }
    
    cep = argv[1];
    
    //buscar endereço via ViaCEP (inclui código IBGE)
    if (buscar_endereco(cep, &endereco) != 0) {
        fprintf(stderr, "\n[ERRO] Não foi possível obter dados do endereço\n");
        return 1;
    }
    
    //buscar dados demográficos via IBGE usando código IBGE obtido
    if (buscar_dados_municipio(endereco.codigo_ibge, &dados_ibge) != 0) {
        fprintf(stderr, "\n[ERRO] Não foi possível obter dados do IBGE\n");
        return 1;
    }
    
    //combino e exibibo dados integrados
    exibir_relatorio_completo(&endereco, &dados_ibge);
    
    return 0;
}
