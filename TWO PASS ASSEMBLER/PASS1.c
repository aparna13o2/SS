#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main() {
    char label[10], opcode[10], operand[10], symbol[10], code[10], mnemonic[3];
    int locctr, start, length, sym_count = 0;
    int error = 0, op_found;
    char error_desc[100];
    char sym_tab[50][50];
    FILE *fp1, *fp2, *fp3, *fp4, *fp5;

    // Open files
    fp1 = fopen("input.txt", "r");
    fp2 = fopen("output.txt", "w");
    fp3 = fopen("optab.txt", "r");
    fp4 = fopen("symtab.txt", "w");
    fp5 = fopen("length.txt", "w");

    if (!fp1 || !fp2 || !fp3 || !fp4 || !fp5) {
        printf("Error opening file.\n");
        return;
    }

    // Read the first line of input
    fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);

    // Check for START directive
    if (strcmp(opcode, "START") == 0) {
        sscanf(operand, "%X", &start);  // Convert start address to hexadecimal
        locctr = start;  // Set location counter
        fprintf(fp2, "\t%s\t%s\t%X\n", label, opcode, locctr);  // Write to output
        fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);  // Read next line
    } else {
        locctr = 0;  // Start at 0 if no START directive
    }

    // Main processing loop
    while (strcmp(opcode, "END") != 0 && error == 0) {
        if (strcmp(label, "#") != 0) {  // Ignore comments
            fprintf(fp2, "%4X\t", locctr);  // Write current locctr to output

            // Check for duplicate symbols
            if (strcmp(label, "*") != 0) {
                for (int i = 0; i < sym_count; i++) {
                    if (strcmp(sym_tab[i], label) == 0) {
                        error = 1;
                        strcat(error_desc, "ERROR: Duplicate Symbol Found: ");
                        strcat(error_desc, label);
                        break;
                    }
                }
                // If no error, add the symbol to the symbol table
                if (!error) {
                    strcpy(sym_tab[sym_count], label);
                    sym_count++;
                    fprintf(fp4, "%s\t%X\n", label, locctr);  // Write symbol to symbol table
                }
            }

            // Process the opcode
            if (strcmp(opcode, "WORD") == 0) {
                locctr += 3;  // Increment locctr by 3 for WORD
            } else if (strcmp(opcode, "RESW") == 0) {
                locctr += (3 * atoi(operand));  // 3 bytes per word
            } else if (strcmp(opcode, "RESB") == 0) {
                locctr += atoi(operand);  // Number of bytes specified
            } else if (strcmp(opcode, "BYTE") == 0) {
                locctr += strlen(operand) - 3;  // Adjust for BYTE length
            } else {
                // Lookup in opcode table
                op_found = 0;
                rewind(fp3);  // Reset file pointer to beginning
                while (fscanf(fp3, "%s\t%s", code, mnemonic) != EOF) {
                    if (strcmp(opcode, code) == 0) {
                        op_found = 1;
                        locctr += 3;  // Increment locctr for instruction
                        break;
                    }
                }

                // If opcode not found
                if (op_found == 0) {
                    error = 1;
                    strcat(error_desc, "ERROR Opcode cannot be found: ");
                    strcat(error_desc, opcode);
                    break;
                }
            }

            // Write instruction to output
            fprintf(fp2, "%s\t%s\t%s\n", label, opcode, operand);
            fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);  // Read next line
        } else {
            fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);  // Skip comments
        }
    }

    // Final output and cleanup
    if (error) {
        printf("%s\n", error_desc);
    } else {
        fprintf(fp2, "%X\t*\t%s\t*\n", locctr, opcode);  // Write final locctr
        length = locctr - start;  // Calculate program length
        printf("The length of the code: %d\n", length);
        fprintf(fp5, "%X", length);  // Write length to file
    }

    // Close files
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);
}
