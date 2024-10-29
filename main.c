#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define ALPHABET_SIZE 26
#define MAX_CODE_LENGTH 100

// 哈夫曼树节点结构
struct MinHeapNode {
    char data;                   // 字符
    unsigned freq;              // 字符频率
    struct MinHeapNode *left, *right; // 左右子树指针
    int is_leaf;                // 1表示叶子节点，0表示内部节点
};

// 最小堆结构
struct MinHeap {
    unsigned size;              // 当前堆大小
    unsigned capacity;          // 堆的容量
    struct MinHeapNode **array; // 指向节点的数组
};

// 哈夫曼编码表
struct HuffmanCode {
    char data;
    char code[MAX_CODE_LENGTH];
};

// 哈夫曼树的根节点（全局变量）
struct MinHeapNode *root = NULL;

// 创建新的哈夫曼树节点
struct MinHeapNode *newNode(char data, unsigned freq) {
    struct MinHeapNode *temp = (struct MinHeapNode *) malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    temp->is_leaf = (data != '$');
    return temp;
}

// 创建最小堆
struct MinHeap *createMinHeap(unsigned capacity) {
    struct MinHeap *minHeap = (struct MinHeap *) malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode **) malloc(minHeap->capacity * sizeof(struct MinHeapNode *));
    return minHeap;
}

// 交换两个最小堆节点
void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b) {
    struct MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

// 维护最小堆的性质
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

// 从最小堆中提取最小节点
struct MinHeapNode *extractMin(struct MinHeap *minHeap) {
    struct MinHeapNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// 插入节点到最小堆
void insertMinHeap(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// 构建最小堆
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

// 构建哈夫曼树
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

// 递归函数来生成哈夫曼编码
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

// 编码字符串
void encodeFile(struct HuffmanCode huffmanCodes[], int size) {
    FILE *inputFile = fopen("ToBeTran.txt", "r");
    if (inputFile == NULL) {
        printf("无法打开文件 ToBeTran.txt\n");
        return;
    }

    FILE *outputFile = fopen("CodeFile.txt", "w");
    if (outputFile == NULL) {
        printf("无法打开文件 CodeFile.txt\n");
        fclose(inputFile);
        return;
    }

    char ch;
    char encodedString[MAX_CODE_LENGTH * 100]; // 假设最大编码字符串长度
    encodedString[0] = '\0'; // 初始化为空字符串
    int found;

    while ((ch = fgetc(inputFile)) != EOF) {
        found = 0; // 初始化 found
        for (int i = 0; i < size; ++i) {
            if (huffmanCodes[i].data == ch) {
                strcat(encodedString, huffmanCodes[i].code); // 添加编码
                fputs(huffmanCodes[i].code, outputFile);
                found = 1; // 找到匹配的字符
                break;
            }
        }
        if (!found) {
            printf("未找到字符 %c 的哈夫曼编码。\n", ch);
        }
    }



    // 打印编码后的数据
    printf("编码后的数据: %s\n", encodedString);


    fclose(inputFile);
    fclose(outputFile);
}

// 解码哈夫曼编码并打印结果
void decodeFile(struct MinHeapNode *root) {
    FILE *inputFile = fopen("CodeFile.txt", "r");
    FILE *outputFile = fopen("TextFile.txt", "w");
    struct MinHeapNode *current = root;
    char ch;

    // 检查文件是否成功打开
    if (inputFile == NULL || outputFile == NULL) {
        printf("无法打开文件 TextFile.txt或CodeFile.txt\n");
        return;
    }

    // 定义一个数组来存储解码的字符
    char decodedCharacters[MAX_CODE_LENGTH * 100]; // 假设最大解码字符数
    int index = 0; // 当前字符数组索引

    while ((ch = fgetc(inputFile)) != EOF) {
        current = (ch == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            // 写入输出文件
            fputc(current->data, outputFile);
            // 打印解码字符到终端
            decodedCharacters[index++] = current->data;
            current = root; // 重置到树的根
        }
    }

    // 打印解码的字符
    printf("解码的字符: ");
    for (int i = 0; i < index; i++) {
        printf("%c", decodedCharacters[i]);
    }
    printf("\n");

    fclose(inputFile);
    fclose(outputFile);
}


// 打印哈夫曼编码
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

// 打印哈夫曼树（文本方式）
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

// 存储哈夫曼树到文件
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

// 读取哈夫曼树从文件
struct MinHeapNode *loadHuffmanTree(FILE *file) {
    char data;
    unsigned freq;
    int is_leaf;
    if (fscanf(file, "%c %u %d\n", &data, &freq, &is_leaf) == EOF) return NULL; // 修复EOF处理
    // 将_转换为空格
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

// 菜单功能实现
void menu() {
    char choice;
    do {
        printf("\n菜单：\n");
        printf("I：初始化（Initialization）\n");
        printf("A：创建ToBeTran.txt 文件\n");
        printf("E：编码（Encoding）\n");
        printf("D：译码（Decoding）\n");
        printf("P：打印代码文件（Print）\n");
        printf("T：打印哈夫曼树（Tree）\n");
        printf("Q：退出（Quit）\n");
        printf("请输入选择: ");
        scanf(" %c", &choice);


        switch (choice) {
            case 'A': {
                FILE *file = fopen("ToBeTran.txt", "w"); // "W" 模式会创建文件（若不存在），并覆盖原来的内容
                if (file == NULL) {
                    printf("无法创建或打开文件 ToBeTran.txt\n");
                    break;
                }
                printf("请输入要写入 ToBeTran.txt 文件的内容（按回车结束）：\n");
                int c;
                while ((c = getchar())!= '\n' && c!= EOF);
                char input[1000]; // 输入内容不会超过 1000 个字符
                fgets(input, sizeof(input), stdin); // 读取一行用户输入
                fputs(input, file); // 将用户输入写入文件
                printf("内容已写入 ToBeTran.txt 文件。\n");

                fclose(file);
                break;
            }

            case 'I': {
                int n;
                printf("输入字符集大小: ");
                scanf("%d", &n);
                char data[n];
                int freq[n];
                for (int i = 0; i < n; i++) {
                    printf("输入字符和对应频率: ");
                    // 先读取字符（包括空格）
                    data[i] = getchar();
                    // 处理可能的换行符（如果有）
                    if (data[i] == '\n') {
                        data[i] = getchar();
                    }
                    scanf("%d", &freq[i]);
                    // 添加调试输出
                    printf("输入的字符: %c, 频率: %d\n", data[i], freq[i]);
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
                // 输出生成的哈夫曼编码
                for (int i = 0; i < index; i++) {
                    printf("字符: %c, 编码: %s\n", huffmanCodes[i].data, huffmanCodes[i].code);
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
                printf("退出程序.\n");
                break;
            }
            default:
                printf("无效选项，请重新输入。\n");
        }
    } while (choice != 'Q');
}

// 主程序
int main() {
    menu();
    return 0;
}
