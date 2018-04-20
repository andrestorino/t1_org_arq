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

void file_read_csv_write_binary(const char *nome_arq_dados, const char *nome_arq_binario)
{
	if(nome_arq_dados != NULL)
	{
		int codigo = 0, escola_size = 0, cidade_size = 0, prestadora_size = 0, total_bytes = 0;
		char byte_padding = 0x00, prestadora[10], data[11], escola[50], cidade[70], uf[3], line[300], *token = NULL;
		HEADER binario_h;
		FILE *csv = NULL, *binario = NULL;
		binario_h.topoPilha = -1;
		binario_h.status = '0';
		
		csv = fopen(nome_arq_dados, "r");
		binario = fopen(nome_arq_binario, "wb");
		fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
		while(1)
		{
			if(feof(csv) != 0)
			{
				break;
			}
			else
			{
				fgets(line, sizeof(line), csv);
				fscanf(csv, "*\n");
				if(line[0] == ';')
				{
					memset(prestadora, 0, sizeof(prestadora));
					prestadora_size = 0;
				}
				else
				{
					sscanf(line, "%10[^;]", prestadora);
					prestadora_size = strlen(prestadora);
				}
				token = strstr(line, ";");
				++token;
				if(token[0] == ';')
				{
					memset(data, -1, 10);
				}
				else
				{
					sscanf(token, "%10[^;]", data);
				}
				token = strstr(token, ";");
				++token;
				sscanf(token, "%d", &codigo);
				token = strstr(token, ";");
				++token;
				if(token[0] == ';')
				{
					memset(escola, 0, sizeof(escola));
					escola_size = 0;
				}
				else
				{
					sscanf(token, "%50[^;\n]", escola);
					escola_size = strlen(escola);
				}
				token = strstr(token, ";");
				++token;
				if(token[0] == ';')
				{
					memset(cidade, 0, sizeof(cidade));
					cidade_size = 0;
				}
				else
				{
					sscanf(token, "%70[^;\n]", cidade);
					cidade_size = strlen(cidade);
				}
				token = strstr(token, ";");
				++token;
				if(token[0] == '\n')
				{
					memset(data, -1, 2);
				}
				else
				{
					sscanf(token, "%2s\n", uf);
				}
				fwrite(&codigo, sizeof(codigo), 1, binario);
				fwrite(&data, strlen(data), 1, binario);
				fwrite(&uf, strlen(uf), 1, binario);
				fwrite(&escola_size, sizeof(escola_size), 1, binario);
				fwrite(&escola, escola_size, 1, binario);
				fwrite(&cidade_size, sizeof(cidade_size), 1, binario);
				fwrite(&cidade, cidade_size, 1, binario);
				fwrite(&prestadora_size, sizeof(prestadora_size), 1, binario);
				fwrite(&prestadora, prestadora_size, 1, binario);
				total_bytes = escola_size + cidade_size + prestadora_size + sizeof(codigo) + strlen(uf) + strlen(data) + 12;
				if(total_bytes < 87)
				{
					fwrite(&byte_padding, (87 - total_bytes), 1, binario);
				}
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

ESCOLA *file_read_all_binary(const char *nome_arq_binario)
{
	ESCOLA *e = NULL;
	if(nome_arq_binario != NULL)
	{

	}
	return e;
}

ESCOLA *file_read_binary_rrn(const char *nome_arq_binario, const int rrn)
{
	ESCOLA *e = NULL;
	if(nome_arq_binario != NULL)
	{
		FILE *binario = NULL;
		HEADER binario_h;
		binario_h.status = '0';
		int campos_variaveis_size = 0, codigoINEP = 0;
		char prestadora[10], data[11], escola[50], cidade[70], uf[3];
		binario = fopen(nome_arq_binario, "rb");
		if(binario != NULL)
		{
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fseek(binario, (IN_DISK_REG_SIZE * (rrn - 1)) + IN_DISK_HEADER_SIZE, SEEK_CUR);
			if(fread(&codigoINEP, sizeof(codigoINEP), 1, binario) > 0)
			{
				fread(data, (sizeof(data) - 1), 1, binario);
				fread(uf, (sizeof(uf) - 1), 1, binario);
				fread(&campos_variaveis_size, sizeof(int), 1, binario);
				fread(escola, campos_variaveis_size, 1, binario);
				fread(&campos_variaveis_size, sizeof(int), 1, binario);
				fread(cidade, campos_variaveis_size, 1, binario);
				fread(&campos_variaveis_size, sizeof(int), 1, binario);
				fread(prestadora, campos_variaveis_size, 1, binario);
				printf("%d ", codigoINEP);
				if(data[0] != -1)
				{
					printf("%s ", data);
				}
				if(uf[0] != -1)
				{
					printf("%s ", uf);
				}
				printf("%d %s %d %s %d %s\n", strlen(escola), escola, strlen(cidade), cidade, strlen(prestadora), prestadora);
			}
			else
			{
				printf("Registro inexistente.\n");
			}
			rewind(binario);
			binario_h.status = '1';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
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
	return e;
}

void file_delete_record(const char *nome_arq_binario, int rrn)
{
	if(nome_arq_binario != NULL)
	{
		HEADER binario_h;
		FILE *binario = NULL;
		char marca = '*';
		int codigoINEP = 0;
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			binario_h.status = '0';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fread(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
			fseek(binario, (rrn - 1) * IN_DISK_REG_SIZE, SEEK_CUR);
			if(fread(&codigoINEP, sizeof(codigoINEP), 1, binario) > 0)
			{
				fseek(binario, -sizeof(codigoINEP), SEEK_CUR);
				fwrite(&marca, sizeof(marca), 1, binario);
				fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
				binario_h.topoPilha = rrn;
			}
			else
			{
				printf("Registro inexistente.\n");
			}
			binario_h.status = '1';
			rewind(binario);
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
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

void file_print_stack(const char *nome_arq_binario)
{
	if(nome_arq_binario != NULL)
	{
		HEADER binario_h;
		FILE *binario = NULL;
		int tmp_pilha = -1;
		char marca = 0x00;
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			binario_h.status = '0';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
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
