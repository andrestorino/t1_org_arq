#ifndef HANDLE_FILE_H
#define HANDLE_FILE_H

        // funcionalidade 1
        void file_read_csv_write_binary(const char *nome_arq_dados, const char *nome_arq_binario); // funcao para ler do csv e escrever no arquivo em binario
        // funcionalidade 2
        void file_read_all_binary(const char *nome_arq_binario); // funcao para recuperar todos os registros do arquivo binario
        // funcionalidade 3
        void file_filter_by_criteria(const char *nome_arq_binario, const char *campo, const char *chave);
        // funcionalidade 4
        void file_read_binary_rrn(const char *nome_arq_binario, const int rrn); // funcao para buscar um registro dado seu RRN
        // funcionalidade 5
        void file_delete_record(const char *nome_arq_binario, int rrn); // funcao para remover um registro dado seu RRN
        // funcionalidade 6
    	void file_add_record(const char *nome_arq_binario, int newCodigoINEP, char *newData, char *newUF, char *newEscola, char *newCidade, char *newPrestadora); // funcao para adicionar um registro
        // funcionalidade 7
        void file_update_rrn(const char *nome_arq_binario, int rrn, int newCodigoINEP, char *newData, char *newUF, char *newEscola, char *newCidade, char *newPrestadora);// funcao para atualizar os campos do registro dado seu RRN
        // funcionalidade 8
        void file_compact(const char *nome_arq_binario);
        // funcionalidade 9
        void file_print_stack(const char *nome_arq_binario); // funcao para imprimir a pilha

#endif
