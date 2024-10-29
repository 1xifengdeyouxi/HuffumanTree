#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define ALPHABET_SIZE 26
#define MAX_CODE_LENGTH 100

// ���������ڵ�ṹ
struct MinHeapNode {
    char data;                   // �ַ�
    unsigned freq;              // �ַ�Ƶ��
    struct MinHeapNode *left, *right; // ��������ָ��
    int is_leaf;                // 1��ʾҶ�ӽڵ㣬0��ʾ�ڲ��ڵ�
};

// ��С�ѽṹ
struct MinHeap {
    unsigned size;              // ��ǰ�Ѵ�С
    unsigned capacity;          // �ѵ�����
    struct MinHeapNode **array; // ָ��ڵ������
};

// �����������
struct HuffmanCode {
    char data;
    char code[MAX_CODE_LENGTH];
};

// ���������ĸ��ڵ㣨ȫ�ֱ�����
struct MinHeapNode *root = NULL;

// �����µĹ��������ڵ�
struct MinHeapNode *newNode(char data, unsigned freq) {
    struct MinHeapNode *temp = (struct MinHeapNode *) malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    temp->is_leaf = (data != '$');
    return temp;
}

// ������С��
struct MinHeap *createMinHeap(unsigned capacity) {
    struct MinHeap *minHeap = (struct MinHeap *) malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode **) malloc(minHeap->capacity * sizeof(struct MinHeapNode *));
    return minHeap;
}

// ����������С�ѽڵ�
void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b) {
    struct MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

// ά����С�ѵ�����
void minHeapify(struct MinHeap *minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq) {
        smallest = left;
    }

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq) {
        smallest = right;
    }

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// ����С������ȡ��С�ڵ�
struct MinHeapNode *extractMin(struct MinHeap *minHeap) {
    struct MinHeapNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// ����ڵ㵽��С��
void insertMinHeap(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// ������С��
struct MinHeap *buildMinHeap(char data[], int freq[], int size) {
    struct MinHeap *minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i) {
        minHeap->array[i] = newNode(data[i], freq[i]);
    }
    minHeap->size = size;
    for (int i = (minHeap->size - 1) / 2; i >= 0; --i) {
        minHeapify(minHeap, i);
    }
    return minHeap;
}

// ������������
struct MinHeapNode *buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeap *minHeap = buildMinHeap(data, freq, size);
    while (minHeap->size > 1) {
        struct MinHeapNode *left = extractMin(minHeap);
        struct MinHeapNode *right = extractMin(minHeap);
        struct MinHeapNode *top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// �ݹ麯�������ɹ���������
void
generateHuffmanCodes(struct MinHeapNode *root, char *code, int top, struct HuffmanCode huffmanCodes[], int *index) {
    if (root->left) {
        code[top] = '0';
        generateHuffmanCodes(root->left, code, top + 1, huffmanCodes, index);
    }
    if (root->right) {
        code[top] = '1';
        generateHuffmanCodes(root->right, code, top + 1, huffmanCodes, index);
    }
    if (!root->left && !root->right) {
        huffmanCodes[*index].data = root->data;
        strncpy(huffmanCodes[*index].code, code, top);
        huffmanCodes[*index].code[top] = '\0'; // null terminate
        (*index)++;
    }
}

// �����ַ���
void encodeFile(struct HuffmanCode huffmanCodes[], int size) {
    FILE *inputFile = fopen("ToBeTran.txt", "r");
    if (inputFile == NULL) {
        printf("�޷����ļ� ToBeTran.txt\n");
        return;
    }

    FILE *outputFile = fopen("CodeFile.txt", "w");
    if (outputFile == NULL) {
        printf("�޷����ļ� CodeFile.txt\n");
        fclose(inputFile);
        return;
    }

    char ch;
    char encodedString[MAX_CODE_LENGTH * 100]; // �����������ַ�������
    encodedString[0] = '\0'; // ��ʼ��Ϊ���ַ���
    int found;

    while ((ch = fgetc(inputFile)) != EOF) {
        found = 0; // ��ʼ�� found
        for (int i = 0; i < size; ++i) {
            if (huffmanCodes[i].data == ch) {
                strcat(encodedString, huffmanCodes[i].code); // ��ӱ���
                fputs(huffmanCodes[i].code, outputFile);
                found = 1; // �ҵ�ƥ����ַ�
                break;
            }
        }
        if (!found) {
            printf("δ�ҵ��ַ� %c �Ĺ��������롣\n", ch);
        }
    }



    // ��ӡ����������
    printf("����������: %s\n", encodedString);


    fclose(inputFile);
    fclose(outputFile);
}

// ������������벢��ӡ���
void decodeFile(struct MinHeapNode *root) {
    FILE *inputFile = fopen("CodeFile.txt", "r");
    FILE *outputFile = fopen("TextFile.txt", "w");
    struct MinHeapNode *current = root;
    char ch;

    // ����ļ��Ƿ�ɹ���
    if (inputFile == NULL || outputFile == NULL) {
        printf("�޷����ļ� TextFile.txt��CodeFile.txt\n");
        return;
    }

    // ����һ���������洢������ַ�
    char decodedCharacters[MAX_CODE_LENGTH * 100]; // �����������ַ���
    int index = 0; // ��ǰ�ַ���������

    while ((ch = fgetc(inputFile)) != EOF) {
        current = (ch == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            // д������ļ�
            fputc(current->data, outputFile);
            // ��ӡ�����ַ����ն�
            decodedCharacters[index++] = current->data;
            current = root; // ���õ����ĸ�
        }
    }

    // ��ӡ������ַ�
    printf("������ַ�: ");
    for (int i = 0; i < index; i++) {
        printf("%c", decodedCharacters[i]);
    }
    printf("\n");

    fclose(inputFile);
    fclose(outputFile);
}


// ��ӡ����������
void printCodeFile() {
    FILE *inputFile = fopen("CodeFile.txt", "r");
    char ch;
    int count = 0;
    while ((ch = fgetc(inputFile)) != EOF) {
        putchar(ch);
        count++;
        if (count % 50 == 0) {
            printf("\n");
        }
    }
    fclose(inputFile);
}

// ��ӡ�����������ı���ʽ��
void printHuffmanTree(struct MinHeapNode *root, int depth) {
    if (!root) return;
    printHuffmanTree(root->right, depth + 1);
    for (int i = 0; i < depth; ++i) printf("\t");
    char dataToPrint = root->data;
    if (dataToPrint == ' ') {
        dataToPrint = '_';
    }
    printf("%c\n", dataToPrint == '$' ? '*' : dataToPrint);
    printHuffmanTree(root->left, depth + 1);
}

// �洢�����������ļ�
void saveHuffmanTree(struct MinHeapNode *root, FILE *file) {
    if (!root) return;
    char dataToSave = root->data;
    if (dataToSave == ' ') {
        dataToSave = '_';
    }
    fprintf(file, "%c %u %d\n", dataToSave, root->freq, root->is_leaf);
    saveHuffmanTree(root->left, file);
    saveHuffmanTree(root->right, file);
}

// ��ȡ�����������ļ�
struct MinHeapNode *loadHuffmanTree(FILE *file) {
    char data;
    unsigned freq;
    int is_leaf;
    if (fscanf(file, "%c %u %d\n", &data, &freq, &is_leaf) == EOF) return NULL; // �޸�EOF����
    // ��_ת��Ϊ�ո�
    if (data == '_') {
        data = ' ';
    }
    struct MinHeapNode *node = newNode(data, freq);
    node->is_leaf = is_leaf;
    if (is_leaf) {
        node->left = node->right = NULL;
    } else {
        node->left = loadHuffmanTree(file);
        node->right = loadHuffmanTree(file);
    }
    return node;
}

// �˵�����ʵ��
void menu() {
    char choice;
    do {
        printf("\n�˵���\n");
        printf("I����ʼ����Initialization��\n");
        printf("A������ToBeTran.txt �ļ�\n");
        printf("E�����루Encoding��\n");
        printf("D�����루Decoding��\n");
        printf("P����ӡ�����ļ���Print��\n");
        printf("T����ӡ����������Tree��\n");
        printf("Q���˳���Quit��\n");
        printf("������ѡ��: ");
        scanf(" %c", &choice);


        switch (choice) {
            case 'A': {
                FILE *file = fopen("ToBeTran.txt", "w"); // "W" ģʽ�ᴴ���ļ����������ڣ���������ԭ��������
                if (file == NULL) {
                    printf("�޷���������ļ� ToBeTran.txt\n");
                    break;
                }
                printf("������Ҫд�� ToBeTran.txt �ļ������ݣ����س���������\n");
                int c;
                while ((c = getchar())!= '\n' && c!= EOF);
                char input[1000]; // �������ݲ��ᳬ�� 1000 ���ַ�
                fgets(input, sizeof(input), stdin); // ��ȡһ���û�����
                fputs(input, file); // ���û�����д���ļ�
                printf("������д�� ToBeTran.txt �ļ���\n");

                fclose(file);
                break;
            }

            case 'I': {
                int n;
                printf("�����ַ�����С: ");
                scanf("%d", &n);
                char data[n];
                int freq[n];
                for (int i = 0; i < n; i++) {
                    printf("�����ַ��Ͷ�ӦƵ��: ");
                    // �ȶ�ȡ�ַ��������ո�
                    data[i] = getchar();
                    // ������ܵĻ��з�������У�
                    if (data[i] == '\n') {
                        data[i] = getchar();
                    }
                    scanf("%d", &freq[i]);
                    // ��ӵ������
                    printf("������ַ�: %c, Ƶ��: %d\n", data[i], freq[i]);
                }

                root = buildHuffmanTree(data, freq, n);
                FILE *file = fopen("hfmTree.txt", "w");
                saveHuffmanTree(root, file);
                fclose(file);
                break;
            }
            case 'E': {
                if (!root) {
                    FILE *file = fopen("hfmTree.txt", "r");
                    root = loadHuffmanTree(file);
                    fclose(file);
                }

                struct HuffmanCode huffmanCodes[ALPHABET_SIZE];
                char code[MAX_CODE_LENGTH];
                int index = 0;
                generateHuffmanCodes(root, code, 0, huffmanCodes, &index);
                // ������ɵĹ���������
                for (int i = 0; i < index; i++) {
                    printf("�ַ�: %c, ����: %s\n", huffmanCodes[i].data, huffmanCodes[i].code);
                }

                encodeFile(huffmanCodes, index);
                break;
            }
            case 'D': {
                if (!root) {
                    FILE *file = fopen("hfmTree.txt", "r");
                    root = loadHuffmanTree(file);
                    fclose(file);
                }
                decodeFile(root);
                break;
            }
            case 'P': {
                printCodeFile();
                break;
            }
            case 'T': {
                if (!root) {
                    FILE *file = fopen("hfmTree.txt", "r");
                    root = loadHuffmanTree(file);
                    fclose(file);
                }
                printHuffmanTree(root, 0);
                break;
            }
            case 'Q': {
                printf("�˳�����.\n");
                break;
            }
            default:
                printf("��Чѡ����������롣\n");
        }
    } while (choice != 'Q');
}

// ������
int main() {
    menu();
    return 0;
}
