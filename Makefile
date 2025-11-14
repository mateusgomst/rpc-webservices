CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lcurl -ljansson

TARGET = integrador_apis
SOURCES = main.c integrador_apis.c
OBJECTS = $(SOURCES:.c=.o)

# Verifica se as bibliotecas necessárias estão instaladas
CURL_CHECK := $(shell pkg-config --exists libcurl && echo yes || echo no)
JANSSON_CHECK := $(shell pkg-config --exists jansson && echo yes || echo no)

all: check-deps $(TARGET)

check-deps:
	@echo "Verificando dependências..."
	@if [ "$(CURL_CHECK)" = "no" ]; then \
		echo "✗ ERRO: libcurl não encontrada"; \
		echo ""; \
		echo "Execute: make install-deps"; \
		echo "ou manualmente: sudo apt-get install libcurl4-openssl-dev"; \
		exit 1; \
	fi
	@if [ "$(JANSSON_CHECK)" = "no" ]; then \
		echo "✗ ERRO: libjansson não encontrada"; \
		echo ""; \
		echo "Execute: make install-deps"; \
		echo "ou manualmente: sudo apt-get install libjansson-dev"; \
		exit 1; \
	fi
	@echo "✓ Todas as dependências encontradas"
	@echo ""

install-deps:
	@echo "═══════════════════════════════════════════════════════"
	@echo "  INSTALANDO DEPENDÊNCIAS"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""
	@echo "As seguintes bibliotecas serão instaladas:"
	@echo "  • libcurl4-openssl-dev  (cliente HTTP/HTTPS)"
	@echo "  • libjansson-dev        (parser JSON)"
	@echo ""
	@echo "Será necessário permissão de administrador (sudo)"
	@echo ""
	sudo apt-get update
	sudo apt-get install -y libcurl4-openssl-dev libjansson-dev pkg-config
	@echo ""
	@echo "✓ Dependências instaladas com sucesso!"
	@echo ""

$(TARGET): $(OBJECTS)
	@echo "Linkando $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Compilação concluída!"

%.o: %.c
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "✓ Limpeza concluída!"

run: $(TARGET)
	./$(TARGET) 01310100

# Executar com diferentes CEPs de teste
test-sp: $(TARGET)
	@echo "Testando com CEP de São Paulo (Av. Paulista)..."
	./$(TARGET) 01310100

test-rj: $(TARGET)
	@echo "Testando com CEP do Rio de Janeiro (Centro)..."
	./$(TARGET) 20040020

test-bh: $(TARGET)
	@echo "Testando com CEP de Belo Horizonte (Centro)..."
	./$(TARGET) 30130100

test-ssa: $(TARGET)
	@echo "Testando com CEP de Salvador (Centro)..."
	./$(TARGET) 40020000

test-floripa: $(TARGET)
	@echo "Testando com CEP de Florianópolis (Centro)..."
	./$(TARGET) 88015100

test-all: test-sp test-rj test-bh test-ssa test-floripa
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "  ✓ Todos os testes concluídos!"
	@echo "═══════════════════════════════════════════════════════"

# Testa as APIs com curl
curl-test-api1:
	@echo "═══════════════════════════════════════════════════════"
	@echo "  Testando API 1 - ViaCEP"
	@echo "═══════════════════════════════════════════════════════"
	@curl -s "https://viacep.com.br/ws/01310100/json/" | jq '.' || \
	curl "https://viacep.com.br/ws/01310100/json/"

curl-test-api2:
	@echo "═══════════════════════════════════════════════════════"
	@echo "  Testando API 2 - IBGE"
	@echo "═══════════════════════════════════════════════════════"
	@curl -s "https://servicodados.ibge.gov.br/api/v1/localidades/municipios/3550308" | jq '.' || \
	curl "https://servicodados.ibge.gov.br/api/v1/localidades/municipios/3550308"

curl-test-api3:
	@echo "═══════════════════════════════════════════════════════"
	@echo "  Testando API 3 - Brasil API (Feriados)"
	@echo "═══════════════════════════════════════════════════════"
	@curl -s "https://brasilapi.com.br/api/feriados/v1/$$(date +%Y)" | jq '.' || \
	curl "https://brasilapi.com.br/api/feriados/v1/$$(date +%Y)"

curl-test-all: curl-test-api1 curl-test-api2 curl-test-api3
	@echo ""
	@echo "✓ Teste de todas as APIs com curl concluído!"

help:
	@echo "═══════════════════════════════════════════════════════"
	@echo "  SISTEMA DE INTEGRAÇÃO DE 3 APIs REST"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""
	@echo "Comandos de compilação:"
	@echo "  make              - Compila o projeto"
	@echo "  make install-deps - Instala dependências necessárias"
	@echo "  make clean        - Remove arquivos gerados"
	@echo ""
	@echo "Comandos de execução:"
	@echo "  make run          - Executa com CEP padrão (São Paulo)"
	@echo "  make test-sp      - Testa com CEP de São Paulo"
	@echo "  make test-rj      - Testa com CEP do Rio de Janeiro"
	@echo "  make test-bh      - Testa com CEP de Belo Horizonte"
	@echo "  make test-ssa     - Testa com CEP de Salvador"
	@echo "  make test-floripa - Testa com CEP de Florianópolis"
	@echo "  make test-all     - Executa todos os testes"
	@echo ""
	@echo "Comandos de teste de APIs (curl):"
	@echo "  make curl-test-api1    - Testa apenas ViaCEP"
	@echo "  make curl-test-api2    - Testa apenas IBGE"
	@echo "  make curl-test-api3    - Testa apenas Brasil API"
	@echo "  make curl-test-all     - Testa todas as APIs"
	@echo ""
	@echo "Dependências necessárias:"
	@echo "  • libcurl4-openssl-dev"
	@echo "  • libjansson-dev"
	@echo "  • pkg-config"
	@echo ""
	@echo "Uso direto:"
	@echo "  ./integrador_apis <CEP>"
	@echo ""
	@echo "Exemplos de CEPs:"
	@echo "  01310100 - São Paulo/SP (Av. Paulista)"
	@echo "  20040020 - Rio de Janeiro/RJ (Centro)"
	@echo "  30130100 - Belo Horizonte/MG (Centro)"
	@echo "  40020000 - Salvador/BA (Centro)"
	@echo "  88015100 - Florianópolis/SC (Centro)"
	@echo ""

.PHONY: all clean run help check-deps install-deps test-sp test-rj test-bh test-ssa test-floripa test-all curl-test-api1 curl-test-api2 curl-test-api3 curl-test-all
