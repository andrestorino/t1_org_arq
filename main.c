/*
	Nome: Andre Luis Storino Junior, N USP: 9293668
	Nome: Francisco Mattos Fortes, N USP: 4590431
	Nome: Leonardo Mendes Bonato, N USP: 9074308
	Nome: Paulo Ricardo J. Miranda, N USP: 10133456
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inc/handle_file.h"

#define FILE_NAME "arquivo.dat" //Nome do arquivo binario

int main(int argc, char *argv[])
{
	char op;	//Codigo da operacao a ser realizada

	if(argc < 2 || strlen(argv[1]) != 1 || (argv[1][0] <= '0' && argv[1][0] > '9')) {
		printf("Erro na entrada de parametros para o programa.\n");
		printf("Uso: ./programaTrab1 CodOp[0-9] +parametros dependentes da operação\n");
		printf("Verifique a documentacao externa para a especificacao de cada operacao\n");
		exit(EXIT_FAILURE);
	}
	else op = argv[1][0];

	switch (op){
		case '1':	//Leitura de registros de um arquivo .csv
			if(argc != 3){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 1 'arquivo.csv'\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_read_csv_write_binary(argv[2], FILE_NAME);
			}
			break;
		case '2':	//Recuperacao de dados de todos os registros
			if(argc != 2){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 2\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_read_all_binary(FILE_NAME);
			}
			break;
		case '3':	//Recuperacao de dados de registros que satisfacam um criterio de busca
			if(argc != 4){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 3 'NomeDoCampo' valor\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_filter_by_criteria(FILE_NAME, argv[2], argv[3]);
			}
			break;
		case '4':	//Busca de um registro por RRN (Relative Register Number)
			if(argc != 3){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 4 RRN\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_read_binary_rrn(FILE_NAME, atoi(argv[2]));
			}
			break;
		case '5':	//Remocao de um registro por RRN (Relative Register Number)
			if(argc != 3){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 5 RRN\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_delete_record(FILE_NAME, atoi(argv[2]));
			}
			break;
		case '6':	//Insercao de um novo registro no arquivo de dados
			if(argc != 8){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 6 valorCampo1 valorCampo2 valorCampo3 valorCampo4 valorCampo5 valorCampo6\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_add_record(FILE_NAME, atoi(argv[2]), argv[3], argv[4], argv[5], argv[6], argv[7]);
			}
			break;
		case '7':	//Atualizacao dos campos de um registro existente no arquivo de dados
			if(argc != 9){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 7 RRN valorCampo1 valorCampo2 valorCampo3 valorCampo4 valorCampo5 valorCampo6\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_update_rrn(FILE_NAME, atoi(argv[2]), atoi(argv[3]), argv[4], argv[5], argv[6], argv[7], argv[8]);
			}
			break;
		case '8':	//Desfragmentacao do arquivo de dados
			if(argc != 2){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 8\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_compact(FILE_NAME);
			}
			break;
		case '9':	//Recupera os RRNs da pilha de registros removidos
			if(argc != 2){
				printf("Erro na entrada de parametros para o programa.\n");
				printf("Uso: ./programaTrab1 9\n");
				exit(EXIT_FAILURE);
			}
			else{
				file_print_stack(FILE_NAME);
			}
			break;
		default:
			exit(EXIT_FAILURE);
			break;
	}

	return 0;
}
