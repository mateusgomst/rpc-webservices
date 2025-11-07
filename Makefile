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

help:
	@echo "Comandos disponíveis:"
	@echo "  make              - Compila o projeto"
	@echo "  make install-deps - Instala dependências necessárias"
	@echo "  make clean        - Remove arquivos gerados"
	@echo "  make run          - Compila e executa com CEP exemplo"
	@echo ""
	@echo "Dependências necessárias:"
	@echo "  • libcurl4-openssl-dev"
	@echo "  • libjansson-dev"
	@echo "  • pkg-config"

.PHONY: all clean run help check-deps install-deps
