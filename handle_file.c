#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/handle_file.h"

// Constantes de pré-processamento
#define IN_DISK_REG_SIZE 87
#define IN_DISK_HEADER_SIZE 5

typedef struct t_header
{
	int topoPilha;
	char status;
} HEADER;

// Funcionalidade 1
void file_read_csv_write_binary(const char *nome_arq_dados, const char *nome_arq_binario)
{
	if(nome_arq_dados != NULL)
	{
		int codigoINEP = 0, escola_size = 0, cidade_size = 0, prestadora_size = 0, total_bytes = 0; // variaveis para guardar o codigo recuperado do arquivo csv e tambem os tamanhos dos campos variaveis
		char byte_padding = '0', prestadora[10], data[11], escola[50], cidade[70], uf[3], line[300], *token = NULL; // variaveis para guardar os campos recuperados de tam fixo e variavel
		HEADER binario_h; // cabecalho do arquivo de dados binario
		FILE *csv = NULL, *binario = NULL;
		binario_h.topoPilha = -1;
		binario_h.status = '0';
		csv = fopen(nome_arq_dados, "r");
		binario = fopen(nome_arq_binario, "wb");
		fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
		while(1)
		{
			strcpy(uf, "00"); // Zera os campos uf, data, escola, cidade e prestadora
			strcpy(data, "0000000000");
			strcpy(escola, "");
			strcpy(cidade, "");
			strcpy(prestadora, "");
			fgets(line, sizeof(line), csv); // le a linha do arquivo csv
			token = line;
			if(feof(csv) == 0) // Se o fim do arquivo nao foi setado
			{
				/*
					As linhas abaixo, de uma forma geral, verificam onde existem campos nulos ou nao, na linha
					lida do arquivo csv. A variavel token e um ponteiro para o caractere ';', entao, a funcao stchr procura a
					primeira ocorrencia do caractere ';' e retorna um ponteiro para a posicao onde o mesmo se encontra.
					Dai, basta apenas verifica se o caractere na posicao 0 do token e o caractere ';'. Se for, indica que o campo e nulo.
					Caso contrario, e um campo nao nulo.
					A funcao sscanf e como a funcao scanf, exceto por trabalhar em strings ao inves de streams.
				*/
				if(token[0] != ';')
				{
					sscanf(token, " %10[^;]", prestadora);
					prestadora_size = strlen(prestadora);
					token = strchr(token, ';');
				}
				else
				{
					prestadora_size = 0;
				}
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %10[^;]", data);
					token = strchr(token, ';');
				}
				++token;
				sscanf(token, "%d", &codigoINEP);
				token = strchr(token, ';');
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %50[^;\n]", escola);
					escola_size = strlen(escola);
					token = strchr(token, ';');
				}
				else
				{
					escola_size = 0;
				}
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %70[^;\n]", cidade);
					cidade_size = strlen(cidade);
					token = strchr(token, ';');
				}
				else
				{
					cidade_size = 0;
				}
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %s", uf);
				}
				fwrite(&codigoINEP, sizeof(codigoINEP), 1, binario);
				fwrite(data, strlen(data), 1, binario);
				fwrite(uf, strlen(uf), 1, binario);
				fwrite(&escola_size, sizeof(int), 1, binario);
				if(escola_size > 0)
				{
					fwrite(escola, escola_size, 1, binario);
				}
				fwrite(&cidade_size, sizeof(int), 1, binario);
				if(cidade_size > 0)
				{
					fwrite(cidade, cidade_size, 1, binario);
				}
				fwrite(&prestadora_size, sizeof(int), 1, binario);
				if(prestadora_size > 0)
				{
					fwrite(prestadora, prestadora_size, 1, binario);
				}
				total_bytes = 28 + escola_size + prestadora_size + cidade_size;
				if(total_bytes < IN_DISK_REG_SIZE)
				{
					fwrite(&byte_padding, sizeof(char), (IN_DISK_REG_SIZE - total_bytes), binario);
				}
			}
			else
			{
				break;
			}
		}
		rewind(binario);
		binario_h.status = '1';
		fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		fclose(binario);
		fclose(csv);
		printf("Arquivo carregado.\n");
	}
	else
	{
		printf("Falha no carregamento do arquivo.\n");
	}
}

// Funcionalidade 2
void file_read_all_binary(const char *nome_arq_binario){
	if(nome_arq_binario != NULL){
		FILE *binario = fopen(nome_arq_binario, "r+b");
		HEADER binario_h;
		int campos_variaveis_size = 0, codigoINEP = 0, reg_size = 28;
		char prestadora[10], data[11], escola[50], cidade[70], uf[3];

		if(binario == NULL) {
			printf("Arquivo inexistente.");
			return;
		}
		fread(&binario_h.status, sizeof(binario_h.status), 1, binario);
		if(binario_h.status == '1'){ // Se o arquivo esta consistente
			binario_h.status = '0';
			rewind(binario);
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fseek(binario, (IN_DISK_HEADER_SIZE - 1), SEEK_CUR);

			while(1) {
				memset(uf, 0x00, sizeof(uf));
				memset(data, 0x00, sizeof(data));
				memset(escola, 0x00, sizeof(escola));
				memset(cidade, 0x00, sizeof(cidade));
				memset(prestadora, 0x00, sizeof(prestadora));
				fread(&codigoINEP, sizeof(codigoINEP), 1, binario);
				if(codigoINEP != -1 && feof(binario) == 0){
					printf("%d ", codigoINEP);
					fread(data, (sizeof(data) - 1), 1, binario);
					printf("%s ", data);
					fread(uf, (sizeof(uf) - 1), 1, binario);
					printf("%s ", uf);
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					reg_size = reg_size + campos_variaveis_size;
					printf("%d ", campos_variaveis_size);
					fread(escola, campos_variaveis_size, 1, binario);
					if(campos_variaveis_size > 0)
					{
						printf("%s ", escola);
					}
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					reg_size = reg_size + campos_variaveis_size;
					printf("%d ", campos_variaveis_size);
					fread(cidade, campos_variaveis_size, 1, binario);
					if(campos_variaveis_size > 0)
					{
						printf("%s ", cidade);
					}
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					reg_size = reg_size + campos_variaveis_size;
					printf("%d ", campos_variaveis_size);
					fread(prestadora, campos_variaveis_size, 1, binario);
					if(campos_variaveis_size > 0)
					{
						printf("%s", prestadora);
					}
					printf("\n");
					if (reg_size < IN_DISK_REG_SIZE && feof(binario) == 0) {
						fseek(binario, IN_DISK_REG_SIZE - reg_size, SEEK_CUR);
					}
					else if(feof(binario) != 0) break;
				}
				else
				{
					if(feof(binario) == 0)
					{
						fseek(binario, IN_DISK_REG_SIZE - sizeof(int), SEEK_CUR);
					}
					else
					{
						break;
					}
				}
				reg_size = 28;
			}
			rewind(binario);
			binario_h.status = '1';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		}
		else // Se o arquivo nao esta consistente
		{
			printf("Arquivo inconsistente.\n");
		}
		fclose(binario);
	}
	else {
		printf("Falha no processamento do arquivo.\n");
	}
}

// Funcionalidade 3
void file_filter_by_criteria(const char *nome_arq_binario, const char *campo, const char *chave) {
	if (nome_arq_binario == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	FILE *binario = fopen(nome_arq_binario, "r+b");

	if(binario == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	HEADER binario_h;
	int campos_variaveis_size = 0, codigoINEP = 0, printRegister = 0, print_flag = 0; // print_flag e usado para saber se algum registro foi printado.
	int escolaChecker = 0, prestadoraChecker = 0, cidadeChecker = 0, reg_size = 28; // Variaveis utilizadas para guardar os tamanhos dos campos variaveis do registro.
	char prestadora[10], data[11], escola[50], cidade[70], uf[3]; // Variaveis utilizadas para a recuperacao dos campos do registro.
	binario = fopen(nome_arq_binario, "r+b");

	if(binario != NULL)
	{
		fread(&binario_h.status, sizeof(binario_h.status), 1, binario);
		if(binario_h.status == '1')
		{
			rewind(binario);
			binario_h.status = '0';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fseek(binario, (IN_DISK_HEADER_SIZE - 1), SEEK_CUR);

			while(1) {
				memset(uf, 0x00, sizeof(uf));
				memset(data, 0x00, sizeof(data));
				memset(escola, 0x00, sizeof(escola));
				memset(cidade, 0x00, sizeof(cidade));
				memset(prestadora, 0x00, sizeof(prestadora));

				fread(&codigoINEP, sizeof(codigoINEP), 1, binario);

				if(codigoINEP != -1 && feof(binario) == 0){
					if(strncmp("codINEP", campo, strlen(campo)) == 0 && codigoINEP == atoi(chave)) {
						printRegister = 1;
					}

					// CAMPOS FIXOS
					fread(data, (sizeof(data) - 1), 1, binario);
					if(strcmp(data, "0000000000") != 0)
					{
						if(strncmp("dataAtiv", campo, strlen(campo)) == 0 && strncmp(data, chave, strlen(chave)) == 0) {
							printRegister = 1;
						}
					}

					fread(uf, (sizeof(uf) - 1), 1, binario);
					if(uf[0] != '0')
					{
						if(strncmp("uf", campo, strlen(campo)) == 0 && strncmp(uf, chave, strlen(chave)) == 0) {
							printRegister = 1;
						}
					}

					// ESCOLA
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					reg_size = reg_size + campos_variaveis_size;
					fread(escola, campos_variaveis_size, 1, binario);
					if(campos_variaveis_size > 0)
					{
						escolaChecker = campos_variaveis_size;
						if(strncmp("nomeEscola", campo, strlen(campo)) == 0 && strncmp(escola, chave, strlen(chave)) == 0) {
							printRegister = 1;
						}
					}

					// CIDADE
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					reg_size = reg_size + campos_variaveis_size;
					fread(cidade, campos_variaveis_size, 1, binario);
					if(campos_variaveis_size > 0)
					{
						cidadeChecker = campos_variaveis_size;
						if(strncmp("municipio", campo, strlen(campo)) == 0 && strncmp(cidade, chave, strlen(chave)) == 0) {
							printRegister = 1;
						}
					}

					// PRESTADORA
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					reg_size = reg_size + campos_variaveis_size;
					fread(prestadora, campos_variaveis_size, 1, binario);
					if(campos_variaveis_size > 0)
					{
						prestadoraChecker = campos_variaveis_size;
						if(strncmp("prestadora", campo, strlen(campo)) == 0 && strncmp(prestadora, chave, strlen(chave)) == 0) {
							printRegister = 1;
						}
					}

					if(printRegister) { // Se encontrou algum registro que satisfaz o criterio
						// PRINT_OUTPUT
						printf("%d ", codigoINEP);
						printf("%s ", data);
						printf("%s ", uf);
						if(escolaChecker > 0) printf("%d %s ", escolaChecker, escola);
						else printf("%d ", escolaChecker);
						if(cidadeChecker > 0) printf("%d %s ", cidadeChecker, cidade);
						else printf("%d ", escolaChecker);
						if(prestadoraChecker > 0) printf("%d %s", prestadoraChecker, prestadora);
						else printf("%d ", prestadoraChecker);
						printf("\n");
						printRegister = 0;
						print_flag = 1;
					}
					if (reg_size < IN_DISK_REG_SIZE && feof(binario) == 0) {
						fseek(binario, IN_DISK_REG_SIZE - reg_size, SEEK_CUR);
					}
					else if(feof(binario) != 0) {
						break;
					}
				}
				else {
					if(feof(binario) == 0)
					{
						fseek(binario, IN_DISK_REG_SIZE - sizeof(int), SEEK_CUR);
					}
					else
					{
						break;
					}
				}
				reg_size = 28;
			}

			if(print_flag == 0) // Se nao houve print (o mesmo que: nao encontrou nenhum registro que satisfaz o criterio).
			{
				printf("Registro inexistente.\n");
			}
			rewind(binario);
			binario_h.status = '1';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		}
		else
		{
			printf("Arquivo inconsistente.\n");
		}
		fclose(binario);
	}
	else
	{
		printf("Falha no processamento do arquivo\n");
	}
}

// Funcionalidade 4
void file_read_binary_rrn(const char *nome_arq_binario, const int rrn)
{
	if(nome_arq_binario != NULL)
	{
		FILE *binario = NULL;
		HEADER binario_h; // cabecalho do arquivo de dados
		int campos_variaveis_size = 0, codigoINEP = 0; // campos_variaveis_size guarda o tamanho dos campos variaveis
		char prestadora[10], data[11], escola[50], cidade[70], uf[3]; // Variaveis utilizadas na recuperacao dos campos do registro
		memset(uf, 0x00, sizeof(uf));
		memset(data, 0x00, sizeof(data));
		memset(prestadora, 0x00, sizeof(prestadora));
		memset(escola, 0x00, sizeof(escola));
		memset(cidade, 0x00, sizeof(cidade));
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			fread(&binario_h.status, sizeof(binario_h.status), 1, binario);
			if(binario_h.status == '1') // Se o arquivo esta consistente
			{
				rewind(binario);
				binario_h.status = '0';
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
				fseek(binario, (IN_DISK_REG_SIZE * (rrn - 1)) + (IN_DISK_HEADER_SIZE - 1), SEEK_CUR);
				if(fread(&codigoINEP, sizeof(codigoINEP), 1, binario) > 0)
				{
					if(codigoINEP != -1)
					{
						printf("%d ", codigoINEP);
						fread(data, (sizeof(data) - 1), 1, binario);
						printf("%s ", data);
						fread(uf, (sizeof(uf) - 1), 1, binario);
						printf("%s ", uf);
						fread(&campos_variaveis_size, sizeof(int), 1, binario);
						printf("%d ", campos_variaveis_size);
						if(campos_variaveis_size > 0)
						{
							fread(escola, campos_variaveis_size, 1, binario);
							printf("%s ", escola);
						}
						fread(&campos_variaveis_size, sizeof(int), 1, binario);
						printf("%d ", campos_variaveis_size);
						if(campos_variaveis_size > 0)
						{
							fread(cidade, campos_variaveis_size, 1, binario);
							printf("%s ", cidade);
						}
						fread(&campos_variaveis_size, sizeof(int), 1, binario);
						printf("%d ", campos_variaveis_size);
						if(campos_variaveis_size > 0)
						{
							fread(prestadora, campos_variaveis_size, 1, binario);
							printf("%s", prestadora);
						}
						printf("\n");
					}
					else
					{
						printf("Registro inexistente.\n");
					}
				}
				else
				{
					printf("Registro inexistente.\n");
				}
				rewind(binario);
				binario_h.status = '1';
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			}
			else // Se o arquivo nao esta consistente
			{
				printf("Arquivo inconsistente.\n");
			}
			fclose(binario);
		}
		else
		{
			printf("Falha no processamento do arquivo.\n");
		}
	}
	else
	{
		printf("Falha no processamento do arquivo.\n");
	}
}

// Funcionalidade 5
void file_delete_record(const char *nome_arq_binario, int rrn)
{
	if(nome_arq_binario != NULL)
	{
		HEADER binario_h;
		FILE *binario = NULL;
		int codigoINEP = 0, marca = -1;
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			fread(&binario_h.status, sizeof(binario_h.status), 1, binario);
			if(binario_h.status == '1')
			{
				rewind(binario);
				binario_h.status = '0';
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);

				fseek(binario, sizeof(binario_h.status), SEEK_SET);

				fread(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
				fseek(binario, (rrn - 1) * IN_DISK_REG_SIZE, SEEK_CUR);
				if(fread(&codigoINEP, sizeof(codigoINEP), 1, binario) > 0 && codigoINEP != -1) // Se e um RRN valido e se o registro nao foi deletado
				{
					fseek(binario, -sizeof(codigoINEP), SEEK_CUR); // Volta o campo de codigo que havia lido
					fwrite(&marca, sizeof(marca), 1, binario); // Marca como registro excluido
					fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario); // Escreve o topo da pilha
					binario_h.topoPilha = rrn; // Guarda o novo topo da pilha
					printf("Registro removido com sucesso.\n");
				}
				else
				{
					printf("Registro inexistente.\n");
				}
				binario_h.status = '1';
				rewind(binario); // Volta o poteiro do arquivo para o comeco do mesmo
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
				fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario); // Atualiza o topo da pilha
			}
			else
			{
				printf("Arquivo inconsistente\n");
			}
			fclose(binario);
		}
		else
		{
			printf("Falha no processamento do arquivo.\n");
		}
	}
	else
	{
		printf("Falha no processamento do arquivo.\n");
	}
}

// Funcionalidade 6
void file_add_record(const char *nome_arq_binario, int newCodigoINEP, char *newData, char *newUF, char *newEscola, char *newCidade, char *newPrestadora)
{
	if(nome_arq_binario != NULL)
	{
		HEADER binario_h;
		FILE *binario = NULL;
		int tmp_pilha = -1, campos_variaveis_size = 0, marca = -1, regsize = 28;
		char bytePadding = '0', nulo = '0';

		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			fread(&binario_h.status, sizeof(binario_h.status), 1, binario);
			if(binario_h.status == '1')
			{
				rewind(binario);
				binario_h.status = '0';
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);

				fseek(binario, sizeof(binario_h.status), SEEK_SET);

				fread(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario); // Recupera o topo da pilha
				tmp_pilha = binario_h.topoPilha;

				if(tmp_pilha != -1) // Caso há algum registro logicamente removido
				{
					fseek(binario, ((binario_h.topoPilha - 1) * IN_DISK_REG_SIZE) + IN_DISK_HEADER_SIZE + sizeof(marca), SEEK_SET); // Pega o novo topo da pilha a partir da poisicao inicial do arquivo
					fread(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);

					fseek(binario, ((tmp_pilha - 1) * IN_DISK_REG_SIZE) + IN_DISK_HEADER_SIZE, SEEK_SET); // Vai para o comeco do registro e calcula o byte offset do registro logicamente removido
					fwrite(&newCodigoINEP, sizeof(newCodigoINEP), 1, binario);
					if(strcmp(newData, "0000000000") == 0)
					{
						for(int i = 0; i < 10; i++)
							fwrite(&nulo, sizeof(char), 1, binario);
					}
					else fwrite(newData, strlen(newData), 1, binario);
					if(newUF[0] == '0')
					{
						for(int i = 0; i < 2; i++)
							fwrite(&nulo, sizeof(char), 1, binario);
					}
					else fwrite(newUF, strlen(newUF), 1, binario);
					campos_variaveis_size = strlen(newEscola);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newEscola, campos_variaveis_size, 1, binario);
					campos_variaveis_size = strlen(newCidade);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newCidade, campos_variaveis_size, 1, binario);
					campos_variaveis_size = strlen(newPrestadora);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newPrestadora, campos_variaveis_size, 1, binario);
					if(regsize < 87) fwrite(&bytePadding, sizeof(char), IN_DISK_REG_SIZE - regsize, binario); // Preenche com o byte padding para o tamnho do registro ser fixo
				}
				else // Caso a pilha esteja vazia, insere no final do arquivo
				{
					fseek(binario, 0, SEEK_END);
					fwrite(&newCodigoINEP, sizeof(newCodigoINEP), 1, binario);
					if(strcmp(newData, "0") == 0) fwrite(&nulo, sizeof(char), 10, binario);
					else fwrite(newData, strlen(newData), 1, binario);
					if(strcmp(newUF, "0") == 0) fwrite(&nulo, sizeof(char), 2, binario);
					else fwrite(newUF, strlen(newUF), 1, binario);
					campos_variaveis_size = strlen(newEscola);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newEscola, campos_variaveis_size, 1, binario);
					campos_variaveis_size = strlen(newCidade);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newCidade, campos_variaveis_size, 1, binario);
					campos_variaveis_size = strlen(newPrestadora);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newPrestadora, campos_variaveis_size, 1, binario);
					if(regsize < 87) fwrite(&bytePadding, sizeof(char), IN_DISK_REG_SIZE - regsize, binario);
				}
				printf("Registro inserido com sucesso.\n");
				binario_h.status = '1';
				rewind(binario);
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
				fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
			}
			else
			{
				printf("Arquivo inconsistente\n");
			}
			fclose(binario);
		}
		else
		{
			printf("Falha no processamento do arquivo.\n");
		}
	}
	else
	{
		printf("Falha no processamento do arquivo.\n");
	}
}

// Funcionalidade 7
void file_update_rrn(const char *nome_arq_binario, int rrn, int newCodigoINEP, char *newData, char *newUF, char *newEscola, char *newCidade, char *newPrestadora){
	char status = '0', bytePadding = '0', nulo = '0';
	FILE *binario = NULL;
	int campos_variaveis_size, isRemoved, regsize;

	binario = fopen(nome_arq_binario, "r+b");
	if(binario != NULL){
		fread(&status, sizeof(status), 1, binario);
		if(status == '1')
		{
			status = '0';
			rewind(binario);
			fwrite(&status, sizeof(status), 1, binario);
			fseek(binario, (IN_DISK_REG_SIZE * (rrn - 1)) + sizeof(int), SEEK_CUR);
			if(fread(&isRemoved, sizeof(int), 1, binario) > 0){
				if(isRemoved != -1){
					fseek(binario, -sizeof(isRemoved), SEEK_CUR);
					regsize = 28;
					fwrite(&newCodigoINEP, sizeof(newCodigoINEP), 1, binario);
					if(strcmp(newData, "0000000000") == 0)
					{
						for(int i = 0; i < 10; i++)
							fwrite(&nulo, sizeof(char), 1, binario);
					}
					else fwrite(newData, strlen(newData), 1, binario);
					if(newUF[0] == '0')
					{
						for(int i = 0; i < 2; i++)
							fwrite(&nulo, sizeof(char), 1, binario);
					}
					else fwrite(newUF, strlen(newUF), 1, binario);
					campos_variaveis_size = strlen(newEscola);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newEscola, campos_variaveis_size, 1, binario);
					campos_variaveis_size = strlen(newCidade);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newCidade, campos_variaveis_size, 1, binario);
					campos_variaveis_size = strlen(newPrestadora);
					regsize += campos_variaveis_size;
					fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
					if(campos_variaveis_size > 0) fwrite(newPrestadora, campos_variaveis_size, 1, binario);
					if(regsize < 87) fwrite(&bytePadding, sizeof(char), IN_DISK_REG_SIZE - regsize, binario);
					printf("Registro alterado com sucesso.\n");
				}
				else{
					printf("Registro inexistente.\n");
				}
			}
			else
			{
				printf("Registro inexistente.\n");
			}
			status = '1';
			rewind(binario);
			fwrite(&status, sizeof(status), 1, binario);
		}
		else
		{
			printf("Arquivo inconsistente\n");
		}
		fclose(binario);
	}
	else{
		printf("Falha no processamento do arquivo.\n");
	}
}

// Funcionalidade 8
void file_compact(const char *nome_arq_binario)
{
	FILE *oldbin = NULL, *newbin = NULL;
	HEADER oldbinario_h, newbinario_h;
	int campos_variaveis_size = 0, codigoINEP = 0, regsize = 28, i = 0;
	char byte_padding = '0', prestadora[20], data[11], escola[50], cidade[70], uf[3];
	char *tmp_arq = "tmp.dat";
	if(nome_arq_binario != NULL)
	{
		oldbin = fopen(nome_arq_binario, "r+b");
		if(oldbin != NULL)
		{
			fread(&oldbinario_h.status, sizeof(oldbinario_h.status), 1, oldbin);
			if(oldbinario_h.status == '1') // Se o arquivo de dados antigo esta consistente, realiza a compactacao
			{
				fread(&oldbinario_h.topoPilha, sizeof(oldbinario_h.topoPilha), 1, oldbin); // Obtem o topo da pilha do arquivo antigo para verificar se necessita realizar a compactacao
				if(oldbinario_h.topoPilha != -1)
				{
					oldbinario_h.status = '0';
					rewind(oldbin);
					fwrite(&oldbinario_h.status, sizeof(oldbinario_h.status), 1, oldbin);
					fseek(oldbin, (IN_DISK_HEADER_SIZE - 1), SEEK_CUR);
					newbin = fopen(tmp_arq, "wb");
					if(newbin != NULL)
					{
						// Escreve o cabecalho do arquivo compactado
						newbinario_h.status = '0';
						newbinario_h.topoPilha = -1;
						fwrite(&newbinario_h.status, sizeof(newbinario_h.status), 1, newbin);
						fwrite(&newbinario_h.topoPilha, sizeof(newbinario_h.topoPilha), 1, newbin);

						while(1)
						{
							memset(uf, 0x00, sizeof(uf));
							memset(data, 0x00, sizeof(data));
							memset(escola, 0x00, sizeof(escola));
							memset(cidade, 0x00, sizeof(cidade));
							memset(prestadora, 0x00, sizeof(prestadora));

							fread(&codigoINEP, sizeof(codigoINEP), 1, oldbin);
							if(codigoINEP != -1 && feof(oldbin) == 0) // Se o registro nao foi removido logicamente no arquivo antigo
							{
								fwrite(&codigoINEP, sizeof(codigoINEP), 1, newbin); // Escreve codigoINEP no arquivo compactado
								fread(data, (sizeof(data) - 1), 1, oldbin); // Recupera o campo data do arquivo de dados antigo
								fwrite(data, (sizeof(data) - 1), 1, newbin); // Escreve o campo data no arquivo de dados compactado
								fread(uf, (sizeof(uf) - 1), 1, oldbin); // Recupera o campo uf do arquivo de dados antigo
								fwrite(uf, (sizeof(uf) - 1), 1, newbin); // Escreve o campo data no arquivo de dados compactado
								fread(&campos_variaveis_size, sizeof(campos_variaveis_size), 1, oldbin); // Recupera o indicador de tamanho do campo nomeEscola do arquivo de dadosantigo
								fwrite(&campos_variaveis_size, sizeof(campos_variaveis_size), 1, newbin); // Escreve o indicador de tamanho do campo nomeEscola do arquivo de dados compactado
								regsize += campos_variaveis_size; // Aumenta o tamanho do registro
								if(campos_variaveis_size > 0) // Se o indicador de nomeEscola for maior que 0, entao escreve o campo nomeEscola no arquivo de dados compactado
								{
									fread(escola, campos_variaveis_size, 1, oldbin);
									fwrite(escola, campos_variaveis_size, 1, newbin);
								}
								fread(&campos_variaveis_size, sizeof(campos_variaveis_size), 1, oldbin); // Recupera o indicador de tamanho do campo municipio do arquivo de dadosantigo
								fwrite(&campos_variaveis_size, sizeof(campos_variaveis_size), 1, newbin); // Escreve o indicador de tamanho do campo municipio do arquivo de dados compactado
								regsize += campos_variaveis_size;
								if(campos_variaveis_size > 0)
								{
									fread(cidade, campos_variaveis_size, 1, oldbin);
									fwrite(cidade, campos_variaveis_size, 1, newbin);
								}
								fread(&campos_variaveis_size, sizeof(campos_variaveis_size), 1, oldbin); // Recupera o indicador de tamanho do campo prestadora do arquivo de dadosantigo
								fwrite(&campos_variaveis_size, sizeof(campos_variaveis_size), 1, newbin); // Escreve o indicador de tamanho do campo prestadora do arquivo de dados compactado
								regsize += campos_variaveis_size;
								if(campos_variaveis_size > 0)
								{
									fread(prestadora, campos_variaveis_size, 1, oldbin);
									fwrite(prestadora, campos_variaveis_size, 1, newbin);
								}
								if(regsize < IN_DISK_REG_SIZE && feof(oldbin) == 0) // Se o registro nao atingiu o tamanho fixo de 87 bytes, adiciona byte padding ao registro do arquivo de dados compactado
								{
									fseek(oldbin, (IN_DISK_REG_SIZE - regsize), SEEK_CUR);
									fwrite(&byte_padding, sizeof(char), (IN_DISK_REG_SIZE - regsize), newbin);
								}
								else if(feof(oldbin) != 0)
								{
									break;
								}
							}
							else
							{
								if(feof(oldbin) == 0)
								{
									fseek(oldbin, IN_DISK_REG_SIZE - sizeof(int), SEEK_CUR);
								}
								else
								{
									break;
								}
							}
							regsize = 28;
						} // While bracket
						newbinario_h.status = '1';
						rewind(newbin);
						fwrite(&newbinario_h.status, sizeof(newbinario_h.status), 1, newbin);
						fclose(newbin);
					}
					else
					{
						printf("Falha no processamento do arquivo newbin\n");
					}
				}
				else
				{
					printf("O arquivo nao necessita ser compactado.\n");
				}
			}
			else
			{
				printf("Arquivo inconsistente\n");
			}
			oldbinario_h.status = '1';
			rewind(oldbin);
			fwrite(&oldbinario_h.status, sizeof(oldbinario_h.status), 1, oldbin);
			fclose(oldbin);
			if(remove(nome_arq_binario) != -1)
			{
				if(rename(tmp_arq, nome_arq_binario) != -1)
				{
					printf("Arquivo de dados compactado com sucesso.\n");
				}
				else
				{
					printf("Nao foi possivel renomear o arquivo compactado\n");
				}
			}
			else
			{
				printf("Nao foi possivel excluir o arquivo de dados antigo\n");
			}
		}
		else
		{
			printf("Falha no processamento do arquivo oldbin\n");
		}
	} // nome_arq_binario != NULL test bracket
}

// Funcionalidade 9
void file_print_stack(const char *nome_arq_binario)
{
	if(nome_arq_binario != NULL)
	{
		HEADER binario_h;
		FILE *binario = NULL;
		int tmp_pilha = -1, marca = -1;
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			fread(&binario_h.status, sizeof(binario_h.status), 1, binario);
			if(binario_h.status == '1')
			{
				rewind(binario);
				binario_h.status = '0';
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);

				fseek(binario, sizeof(binario_h.status), SEEK_SET);

				fread(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
				tmp_pilha = binario_h.topoPilha;
				if(tmp_pilha != -1)
				{
					while(tmp_pilha != -1)
					{
						printf("%d ", tmp_pilha);
						fseek(binario, ((tmp_pilha - 1) * IN_DISK_REG_SIZE) + IN_DISK_HEADER_SIZE, SEEK_SET);
						fread(&marca, sizeof(marca), 1, binario);
						fread(&tmp_pilha, sizeof(tmp_pilha), 1, binario);
					}
				}
				else
				{
					printf("Pilha vazia.");
				}
				printf("\n");
				binario_h.status = '1';
				rewind(binario);
				fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
				fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
			}
			else
			{
				printf("Arquivo inconsistente\n");
			}
			fclose(binario);
		}
		else
		{
			printf("Falha no processamento do arquivo.\n");
		}
	}
	else
	{
		printf("Falha no processamento do arquivo.\n");
	}
}
