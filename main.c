#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image\stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image\stb_image_write.h"

struct Node
{
    int data;
    char* dna;
    struct Node* left;
    struct Node* right;
};
typedef struct Node* BST;

char key[16][8]; // Secret key
int pic[2000][2000]; // Input image
int I = 0; // x[I]
double x; // Initialize x_0
int A; // Plain image A x B
int B; // Plain image A x B
int comp; // Number of components of input image (1: greyscale)
int N; // Number of candidate BST nodes
const double R = 3.99;

// Image Processing
void inputImg (char* inputPic);
void outputImg (int** cipherImg);
// Initialize
void generateKey ();
char xorNum (char x, char y);
double divideTwo (int x, int num);
void cal_x ();
void cal_N ();
void update_ChaoticMap ();
// Table 3
BST createNode (int data);
void insert (BST bst, int num);
BST generateBST ();
// Table 4
char* intToBinary (int num); 
char* binaryToDNA (char* binary, int rule);
void traversal (BST node);
BST convertBSTtoDNA (BST bst);
// Table 5
void convertImageToDNA(int** img, char*** dnaImg);
// Table 6
char XOR(char x, char y);
char* XORstr (char* x, char* y);
void allocation (BST bst, char*** dnaCipherImg, int** visited, int lx, int ly);
int allVisited (int** visited);
char*** DNACipherImg (BST dnaBst, char*** dnaImg);
// Table 7
void copy2char (char* c, int loc, char* twoBin);
char* DNAToBinary (char* DNA, int rule);
int binaryToInt (char* binary);
int** cipherImg (char*** DNA_cipherImg);

int main(void)
{
    char* inputPic = "inputImg.png";
    inputImg(inputPic);
    printf("A = %d. B = %d.\n", A, B);
    generateKey();
    cal_x();
    printf("A = %d. B = %d. x = %lf.\n", A, B, x);
    cal_N();
    printf("A = %d. B = %d. x = %lf. N = %d.\n", A, B, x, N);
    /* code */
    return 0;
}

// Image Processing

void inputImg (char* inputPic)
{
    int width, height, channels;    
    unsigned char *img = stbi_load(inputPic, &width, &height, &channels, 0);
    A = height;
    B = width;
    comp = channels;
    if(img == NULL) 
    {
        printf("Error in loading the image.\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels.\n", width, height, channels);
    int i, j;
    for(i = 1; i <= height; i++)
        for(j = 1; j <= width; j++)
            pic[i][j] = (int)((uint8_t)(*((uint8_t*)img + i*width + j)));
}

void outputImg (int** cipherImg)
{
    size_t img_size = A * B * comp;
    unsigned char *img = malloc(img_size);
    int i, j;
    for(i = 0; i < A; i++)
        for(j = 0; j < B; j++)
            *((uint8_t*)img + i*B + j) =  (uint8_t)(cipherImg[i][j]);
    stbi_write_png("outputImg.png", B, A, comp, img, B * A * comp);
}

// Initialize

void generateKey ()
{
    int i, j;
    for(i = 0; i < 16; i++)
    {
        for(j = 0; j < 8; j++)
        {
            key[i][j] = (rand() % 2) + '0';
            printf("%c ", key[i][j]);
        }
        printf("\n");
    }
}

char xorNum (char x, char y)
{
    if((x == '0' && y == '0') || (x == '1' && y == '1')) return '0';
    if((x == '1' && y == '0') || (x == '0' && y == '1')) return '1';
    return 'x';
}

double divideTwo (int x, int num)
{
    double result = (double)x;
    int i;
    for (i = 0; i < num; i++)
    {
        result = result / 2.;
    }
    return result;
}

void cal_x ()
{
    int i, j;
    double leftSide = 0;
    char keyLeft[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};
    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 8; j++)
        {
            keyLeft[j] = xorNum(keyLeft[j], key[i][j]);
        }
    }
    for(j = 0; j < 8; j++)
    {
        leftSide += (double)((int)(keyLeft[j] - '0') * pow(2., 8 - 1 - j));
    }
    leftSide = leftSide / 256.;
    printf("leftSide = %lf.\n", leftSide);

    double rightSide = 0;
    for(i = 8; i < 16; i++)
    {
        for(j = 0; j < 8; j++)
        {
            rightSide += divideTwo((int)(key[i][j] - '0'), j + 1 + (8 * (i - 8)));
        }
    }
    printf("rightSide = %lf.\n", rightSide);
    x = (leftSide + rightSide) / 2.;
}

void cal_N ()
{
    N = round(x * A);
}

void update_ChaoticMap ()
{
    x = R * x * (1 - x);
    I++;
}

// TABLE 3

BST createNode (int data) 
{
    BST newNode = (BST)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->dna = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void insert (BST bst, int num) 
{
    if (bst == NULL) 
    {
        bst = createNode(num);
    }
    else
    {
        BST current = bst;
        while (current != NULL) 
        {
            if (num < current->data) 
            {
                if (current->left == NULL) 
                {
                    current->left = createNode(num);
                    break;
                } 
                else 
                {
                    current = current->left;
                }
            } 
            else 
            {
                if (current->right == NULL) 
                {
                    current->right = createNode(num);
                    break;
                } 
                else 
                {
                    current = current->right;
                }
            }
        }
    }
}

BST generateBST ()
{
    int i;
    BST bst = (BST)malloc(sizeof(struct Node));
    for (i = 1; i <= N; i++) 
    {
        update_ChaoticMap();
        int num = round(x * 255);
        insert(bst, num);
    }
    return bst;
}

// TABLE 4

char* intToBinary (int num)
{
    int i;
    char* binary = (char*)malloc(9 * sizeof(char));
    binary[8] = '\0';
    for (i = 7; i >= 0; i--) 
    {
        binary[i] = (num % 2) + '0';
        num /= 2;
    }
    return binary;
}

char* binaryToDNA(char* binary, int rule) 
{
    int i; 
    int length = strlen(binary);
    char* result = malloc((length / 2) + 1);
    for (i = 0; i < length; i += 2) 
    { 
        char a = binary[i];
        char b = binary[i + 1];
        switch (rule) 
        {
            case 1:
                if (a == '0' && b == '0') result[i / 2] = 'A';
                else if (a == '1' && b == '1') result[i / 2] = 'T';
                else if (a == '1' && b == '0') result[i / 2] = 'C';
                else if (a == '0' && b == '1') result[i / 2] = 'G';
                break;
            case 2:
                if (a == '0' && b == '0') result[i / 2] = 'A';
                else if (a == '1' && b == '1') result[i / 2] = 'T';
                else if (a == '0' && b == '1') result[i / 2] = 'C';
                else if (a == '1' && b == '0') result[i / 2] = 'G';
                break;
            case 3:
                if (a == '1' && b == '1') result[i / 2] = 'A';
                else if (a == '0' && b == '0') result[i / 2] = 'T';
                else if (a == '1' && b == '0') result[i / 2] = 'C';
                else if (a == '0' && b == '1') result[i / 2] = 'G';
                break;
            case 4:
                if (a == '1' && b == '1') result[i / 2] = 'A';
                else if (a == '0' && b == '0') result[i / 2] = 'T';
                else if (a == '0' && b == '1') result[i / 2] = 'C';
                else if (a == '1' && b == '0') result[i / 2] = 'G';
                break;
            case 5:
                if (a == '1' && b == '0') result[i / 2] = 'A';
                else if (a == '0' && b == '1') result[i / 2] = 'T';
                else if (a == '0' && b == '0') result[i / 2] = 'C';
                else if (a == '1' && b == '1') result[i / 2] = 'G';
                break;
            case 6:
                if (a == '0' && b == '1') result[i / 2] = 'A';
                else if (a == '1' && b == '0') result[i / 2] = 'T';
                else if (a == '0' && b == '0') result[i / 2] = 'C';
                else if (a == '1' && b == '1') result[i / 2] = 'G';
                break;
            case 7:
                if (a == '1' && b == '0') result[i / 2] = 'A';
                else if (a == '0' && b == '1') result[i / 2] = 'T';
                else if (a == '1' && b == '1') result[i / 2] = 'C';
                else if (a == '0' && b == '0') result[i / 2] = 'G';
                break;
            case 8:
                if (a == '0' && b == '1') result[i / 2] = 'A';
                else if (a == '1' && b == '0') result[i / 2] = 'T';
                else if (a == '1' && b == '1') result[i / 2] = 'C';
                else if (a == '0' && b == '0') result[i / 2] = 'G';
                break;
            default:
                free(result);
                return NULL; 
        }
    }
    result[length / 2] = '\0';
    return result;
}

void traversal(BST node) 
{
    if (node == NULL)
        return;
    traversal(node->left);
    
    char* binary = intToBinary(node->data);
    update_ChaoticMap();
    int ruleNum = round(x * 7) + 1;
    node->dna = binaryToDNA(binary, ruleNum);

    traversal(node->right);
}

BST convertBSTtoDNA(BST bst) 
{
    traversal(bst);
    return bst;
}

// TABLE 5

void convertImageToDNA(int** img, char*** dnaImg)
{
    int i, j;
    for(i = 1; j <= A; j++)
    {
        for(j = 1; j <= B; j++)
        {
            int pixel = pic[i][j];
            char* binImg = intToBinary(pixel);
            update_ChaoticMap();
            int ruleNum = round(x * 7) + 1;
            char* DNA = binaryToDNA(binImg, ruleNum);
            strcpy(dnaImg[i][j], DNA);
            free(binImg);
            free(DNA);
        }
    }
}

// TABLE 6

char XOR (char x, char y)
{
    char result;
    int int_x = (int)x;
    int int_y = (int)y;
    switch (int_x)
    {
        case (int)('A'):
            switch (int_y)
            {
                case (int)('A'): result = 'A';
                case (int)('T'): result = 'T';
                case (int)('C'): result = 'C';
                case (int)('G'): result = 'G';
            }
        case (int)('T'):
            switch (int_y)
            {
                case (int)('A'): result = 'T';
                case (int)('T'): result = 'A';
                case (int)('C'): result = 'G';
                case (int)('G'): result = 'C';
            }
        case (int)('C'):
            switch (int_y)
            {
                case (int)('A'): result = 'C';
                case (int)('T'): result = 'G';
                case (int)('C'): result = 'A';
                case (int)('G'): result = 'T';
            }
        case (int)('G'):
            switch (int_y)
            {
                case (int)('A'): result = 'G';
                case (int)('T'): result = 'C';
                case (int)('C'): result = 'T';
                case (int)('G'): result = 'A';
            }
    }
    return result;
}

char* XORstr (char* x, char* y) // Given strlen(x) = strlen(y)
{
    int length = strlen(x);
    char* result = (char*)malloc(sizeof(char) * (length + 1));
    int i;
    for(i = 0; i < length; i++)
    {
        result[i] = XOR(x[i], y[i]);
    }
    return result;
}

void allocation (BST bst, char*** dnaCipherImg, int** visited, int lx, int ly)
{
    if(lx < 1 || lx > A || ly < 1 || ly > B) return;
    while(bst != NULL)
    {
        visited[lx][ly] = 1;
        strcpy(dnaCipherImg[lx][ly], XORstr(dnaCipherImg[lx][ly], bst->dna));
        allocation(bst->left, dnaCipherImg, visited, lx + 1, ly - 1);
        allocation(bst->right, dnaCipherImg, visited, lx + 1, ly + 1);
    }
}

int allVisited (int** visited)
{
    int check = 0;
    int i, j;
    for(i = 1; i <= A; i++)
    {
        for(j = 1; j <= B; j++)
        {
            if(visited[i][j] == 1) check++;
        }
    }
    if(check == A * B) return 1;
    return 0;
}

char*** DNACipherImg (BST dnaBst, char*** dnaImg)
{
    char*** dnaCipherImg = dnaImg;
    int** visited = {0};
    int lx, ly;
    do
    {
        update_ChaoticMap();
        lx = round(x * (A - 1) + 1);
        update_ChaoticMap();
        ly = round(x * (B - 1) + 1);
        allocation(dnaBst, dnaCipherImg, visited, lx, ly);
    } 
    while (!allVisited(visited));
    return dnaCipherImg;
}

// TABLE 7

void copy2char (char* c, int loc, char* twoBin) // loc is position of DNA [0,3]
{
    c[loc * 2] = twoBin[0];
    c[loc * 2 + 1] = twoBin[1];
}

char* DNAToBinary (char* DNA, int rule)
{
    int length = strlen(DNA);
    char* result = (char*)malloc(length * 2 + 1);
    int i;
    for(i = 0; i < length; i++)
    {
        switch(rule)
        {
            case 1:
                if(DNA[i] == 'A') copy2char(result, i, "00");
                else if(DNA[i] == 'T') copy2char(result, i, "11");
                else if(DNA[i] == 'C') copy2char(result, i, "10");
                else if(DNA[i] == 'G') copy2char(result, i, "01");
            case 2:
                if(DNA[i] == 'A') copy2char(result, i, "00");
                else if(DNA[i] == 'T') copy2char(result, i, "11");
                else if(DNA[i] == 'C') copy2char(result, i, "01");
                else if(DNA[i] == 'G') copy2char(result, i, "10");
            case 3:
                if(DNA[i] == 'A') copy2char(result, i, "11");
                else if(DNA[i] == 'T') copy2char(result, i, "00");
                else if(DNA[i] == 'C') copy2char(result, i, "10");
                else if(DNA[i] == 'G') copy2char(result, i, "01");
            case 4:
                if(DNA[i] == 'A') copy2char(result, i, "11");
                else if(DNA[i] == 'T') copy2char(result, i, "00");
                else if(DNA[i] == 'C') copy2char(result, i, "01");
                else if(DNA[i] == 'G') copy2char(result, i, "10");
            case 5:
                if(DNA[i] == 'A') copy2char(result, i, "10");
                else if(DNA[i] == 'T') copy2char(result, i, "01");
                else if(DNA[i] == 'C') copy2char(result, i, "00");
                else if(DNA[i] == 'G') copy2char(result, i, "11");
            case 6:
                if(DNA[i] == 'A') copy2char(result, i, "01");
                else if(DNA[i] == 'T') copy2char(result, i, "10");
                else if(DNA[i] == 'C') copy2char(result, i, "00");
                else if(DNA[i] == 'G') copy2char(result, i, "11");
            case 7:
                if(DNA[i] == 'A') copy2char(result, i, "10");
                else if(DNA[i] == 'T') copy2char(result, i, "01");
                else if(DNA[i] == 'C') copy2char(result, i, "11");
                else if(DNA[i] == 'G') copy2char(result, i, "00");
            case 8:
                if(DNA[i] == 'A') copy2char(result, i, "01");
                else if(DNA[i] == 'T') copy2char(result, i, "10");
                else if(DNA[i] == 'C') copy2char(result, i, "11");
                else if(DNA[i] == 'G') copy2char(result, i, "00");
            default:
                free(result);
                return NULL;
        }
    }
    result[strlen(DNA) * 2] = '\0';
    return result;
}

int binaryToInt (char* binary)
{
    int i;
    int length = strlen(binary);
    int num = 0;
    for (i = length - 1; i >= 0; i--) 
    {
        num += (binary[i] - '0') * (int)(pow(2,length - i - 1));
    }
    return num;
}

int** cipherImg (char*** DNA_cipherImg)
{
    int **cipher_Img = (int**)malloc(A*B*sizeof(int));
    int i, j;
    for(i = 1; i <= A; i++)
    {
        for(j = 1; j <= B; j++)
        {
            update_ChaoticMap();
            int ruleNum = round(x * 7) + 1;
            char* cipherBin = DNAToBinary(DNA_cipherImg[i][j], ruleNum);
            *((int*)cipher_Img + i*B + j) = binaryToInt(cipherBin);
        }
    }
    return cipher_Img;
}