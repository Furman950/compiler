void copy_from_to(FILE *from, FILE *to);
void run_assembler();

FILE *open_file(const char *path, const char *args);
char *read_file_into_buffer(FILE *fp);

char *get_operator(int op_value);

int operator_to_enum(char c);