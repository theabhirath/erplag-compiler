#include "../ast/ast.h"
#include "../ast/symbol_table.h"

typedef enum OPERATOR
{
    OP_ASSIGN,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_AND,
    OP_OR,
    OP_JMP,
    OP_JZ,
    OP_JNZ,
    OP_NEGATE,
    OP_SET,
    OP_RESET,
    OP_CMP,
    OP_LABEL,
    OP_JL,
    OP_JG,
    OP_JLE,
    OP_JGE,
    OP_INC,
    OP_DEC
} OPERATOR;

typedef struct three_ac
{
    char *label;
    OPERATOR op;
    char *target_label;
    ST_ENTRY *arg1;
    ST_ENTRY *arg2;
    ST_ENTRY *result;
    struct three_ac *next;
} three_ac;

typedef struct three_ac_list
{
    struct three_ac *head;
    struct three_ac *tail;
} three_ac_list;

void add_to_three_ac_list(three_ac_list *list, three_ac *node)
{
    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }

        print_three_ac_list(list, stdout);
}

ST_ENTRY *get_temp_var(symbol_table *symtab, enum TYPE type)
{
    ST_ENTRY *temp = malloc(sizeof(ST_ENTRY));
    temp->entry_type = VAR_SYM;
    temp->data.var->type = type;
    temp->data.var->offset = symtab->offset;
    symtab->offset += 4;
    return temp;
}
ST_ENTRY *const_true;
ST_ENTRY *const_false;

void init_three_ac_list(three_ac_list *list)
{
    list->head = NULL;
    list->tail = NULL;

    const_true = malloc(sizeof(ST_ENTRY));
    const_true->entry_type = VAR_SYM;
    const_true->data.var = malloc(sizeof(struct var_entry));
    const_true->data.var->type = __BOOL__;
    const_true->data.var->offset = 0;
    const_true->data.var->val.boolValue = 1;

    const_false = malloc(sizeof(ST_ENTRY));
    const_false->entry_type = VAR_SYM;
    const_false->data.var = malloc(sizeof(struct var_entry));
    const_false->data.var->type = __BOOL__;
    const_false->data.var->offset = 0;
    const_false->data.var->val.boolValue = 0;
}

char *get_label()
{
    static int labels = 0;
    char *label = malloc(sizeof(char) * 10);
    sprintf(label, "label%d", labels);
    labels++;
    return label;
}

three_ac *generate_opcode(ast_node *node, symbol_table *st, three_ac_list *list)
{
    three_ac *temp = malloc(sizeof(three_ac));
    temp->label = NULL;
    temp->arg1 = NULL;
    temp->arg2 = NULL;
    temp->result = NULL;
    temp->next = NULL;
    temp->target_label = NULL;

    switch (node->nodeType)
    {
    case PROGRAM_AST:
    {
        printf("Generating code for program \n");
        fflush(stdout);
        struct programAuxInfo *programAux = (struct programAuxInfo *)node->aux_info;
        ast_node *moduleDeclaration = programAux->ModDec;
        ast_node *otherModules1 = programAux->OtherMod1;
        ast_node *driverModule = programAux->DriverMod;
        ast_node *otherModules2 = programAux->OtherMod2;

        ST_ENTRY *driver_st_entry = checkAllSymbolTables(st, "driver");
        symbol_table *driver_st = driver_st_entry->data.block->symTable;
        LinkedListASTNode *driver_list = driver_st_entry->data.block->body;
        while (driver_list != NULL)
        {
            printf("Statement \n");
            generate_opcode(driver_list->data, driver_st, list);
            driver_list = driver_list->next;
        }
        break;

    }
    case DRIVER_MODULE_AST:
    {
        printf("Generating code for driver module \n");
        fflush(stdout);
        LinkedListASTNode *list = (LinkedListASTNode *)node->aux_info;
        
        ast_node *driverModuleDef = list->data;
        ast_node *nextNode = list->next->data;
        generate_opcode(nextNode, st, list);        
        break;
    }
    case EQUALS_AST:
    {
        temp->op = OP_ASSIGN;
        three_ac *temp1 = generate_opcode(node->right, st, list);
        temp->arg1 = temp1->result;
        temp->arg2 = NULL;
        parse_tree_node *res = node->left;
        temp->result = checkAllSymbolTables(st, res->leafNodeInfo.lexeme);
        add_to_three_ac_list(list, temp);
        break;
    }
    case PLUS_AST:
    {
        temp->op = OP_ADD;
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);
        temp->arg1 = temp1->result;
        temp->arg2 = temp2->result;
        temp->result = get_temp_var(st, getType(temp1->result, st));
        add_to_three_ac_list(list, temp);
        break;
    }
    case MINUS_AST:
    {
        if (node->left != NULL)
        {
            temp->op = OP_SUB;
            three_ac *temp1 = generate_opcode(node->left, st, list);
            three_ac *temp2 = generate_opcode(node->right, st, list);
            temp->arg1 = temp1->result;
            temp->arg2 = temp2->result;
            temp->result = get_temp_var(st, getType(temp1->result, st));
            add_to_three_ac_list(list, temp);
        }
        else
        {
            temp->op = OP_NEGATE;
            three_ac *temp1 = generate_opcode(node->right, st, list);
            temp->arg1 = temp1->result;
            temp->arg2 = NULL;
            temp->result = get_temp_var(st, getType(temp1->result, st));
            add_to_three_ac_list(list, temp);
        }
        break;
    }
    case MUL_AST:
    {
        temp->op = OP_MUL;
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);
        temp->arg1 = temp1->result;
        temp->arg2 = temp2->result;
        temp->result = get_temp_var(st, getType(temp1->result, st));
        add_to_three_ac_list(list, temp);
        break;
    }
    case DIV_AST:
    {
        temp->op = OP_DIV;
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);
        temp->arg1 = temp1->result;
        temp->arg2 = temp2->result;
        temp->result = get_temp_var(st, getType(temp1->result, st));
        add_to_three_ac_list(list, temp);
        break;
    }
    case AND_AST:
    {
        temp->op = OP_AND;
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);
        temp->arg1 = temp1->result;
        temp->arg2 = temp2->result;
        temp->result = get_temp_var(st, getType(temp1->result, st));
        add_to_three_ac_list(list, temp);
        break;
    }
    case OR_AST:
    {
        temp->op = OP_OR;
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);
        temp->arg1 = temp1->result;
        temp->arg2 = temp2->result;
        temp->result = get_temp_var(st, getType(temp1->result, st));
        add_to_three_ac_list(list, temp);
        break;
    }
    /*
        eq

        assembly:
        cmp arg1, arg2
        jz label
        mov result, 0
        jmp label2
        label:
        mov result, 1
        label2:


    */
    case EQ_AST:
    case NE_AST:
    case LT_AST:
    case GT_AST:
    case LE_AST:
    case GE_AST:
    {
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);

        three_ac *first = temp;
        first->label = NULL;
        first->op = OP_CMP;
        first->arg1 = temp1->result;
        first->arg2 = temp2->result;
        first->result = NULL;
        add_to_three_ac_list(list, first);

        three_ac *second = malloc(sizeof(three_ac));
        second->label = NULL;
        switch (node->nodeType)
        {
        case EQ_AST:
            second->op = OP_JZ;
            break;
        case NE_AST:
            second->op = OP_JNZ;
            break;
        case LT_AST:
            second->op = OP_JL;
            break;
        case GT_AST:
            second->op = OP_JG;
            break;
        case LE_AST:
            second->op = OP_JLE;
            break;
        case GE_AST:
            second->op = OP_JGE;
            break;
        }
        second->target_label = get_label();
        add_to_three_ac_list(list, second);

        three_ac *third = malloc(sizeof(three_ac));
        third->label = NULL;
        third->op = OP_RESET;
        third->arg1 = get_temp_var(st, __NUM__);
        third->arg2 = NULL;
        third->result = third->arg1;
        add_to_three_ac_list(list, third);

        three_ac *fourth = malloc(sizeof(three_ac));
        fourth->label = NULL;
        fourth->op = OP_JMP;
        fourth->target_label = get_label();
        add_to_three_ac_list(list, fourth);

        three_ac *fifth = malloc(sizeof(three_ac));
        fifth->label = second->target_label;
        fifth->op = OP_SET;
        fifth->arg1 = third->arg1;
        fifth->arg2 = NULL;
        fifth->result = third->arg1;
        add_to_three_ac_list(list, fifth);

        three_ac *sixth = malloc(sizeof(three_ac));
        sixth->label = fourth->target_label;
        sixth->op = OP_LABEL;
        add_to_three_ac_list(list, sixth);
        return third->arg1;
    }
    /*
        Code:
        while (condition)
        start
            body
        end

        assembly:
        label0:
            cmp condition, 0
            jz label1
            body
            jmp label0
        label1:

    */
    case WHILE_AST:
    {
        printf("In while\n");
        three_ac *first = temp;
        first->label = get_label();
        printf("Label: %s\n", first->label);
        first->op = OP_LABEL;
        add_to_three_ac_list(list, first);

        three_ac *second = malloc(sizeof(three_ac));
        second->label = NULL;
        second->op = OP_CMP;
        second->arg1 = generate_opcode(node->left, st, list)->result;
        second->arg2 = const_false;
        add_to_three_ac_list(list, second);

        three_ac *third = malloc(sizeof(three_ac));
        third->label = NULL;
        third->op = OP_JZ;
        third->target_label = get_label();
        add_to_three_ac_list(list, third);

        generate_opcode(node->right, st, list);

        three_ac *fourth = malloc(sizeof(three_ac));
        fourth->label = NULL;
        fourth->op = OP_JMP;
        fourth->target_label = first->label;
        add_to_three_ac_list(list, fourth);

        three_ac *fifth = malloc(sizeof(three_ac));
        fifth->label = third->target_label;
        fifth->op = OP_LABEL;
        add_to_three_ac_list(list, fifth);
        break;
    }
        /*
            code:
                for (ind in st .. en)
                    body

            assembly:
            label0: cmp ind, en
                    jle label1
                    body
                    inc ind
                    jmp label0
            label1:
        */
    case TRUE_PTN_AST:
    {
        temp->op = OP_SET;
        temp->arg1 = const_true;
        temp->arg2 = NULL;
        temp->result = get_temp_var(st, __BOOL__);
        add_to_three_ac_list(list, temp);
        return temp->result;
    }
    case FOR_AST:
    {
        char *name = malloc(sizeof(char) * 10);
        sprintf(name, "%p", node);
        ST_ENTRY *st_entry = checkSymbolTable(st, name);
        symbol_table *new_st = st_entry->data.block->symTable;

        ST_ENTRY *start_ste = checkSymbolTable(new_st, "$start");
        ST_ENTRY *end_ste = checkSymbolTable(new_st, "$end");
        int start = start_ste->data.var->val.numValue;
        int end = end_ste->data.var->val.numValue;

        parse_tree_node *loop_var_ptn = node->aux_info;

        ST_ENTRY *loop_var_ste = checkSymbolTable(new_st, loop_var_ptn->leafNodeInfo.lexeme);

        if (start < end)
        {
            three_ac *first = temp;
            first->label = get_label();
            first->op = OP_LABEL;
            add_to_three_ac_list(list, first);

            three_ac *second = malloc(sizeof(three_ac));
            second->label = NULL;
            second->op = OP_CMP;
            second->arg1 = loop_var_ste;
            second->arg2 = end_ste;
            add_to_three_ac_list(list, second);

            three_ac *third = malloc(sizeof(three_ac));
            third->label = NULL;
            third->op = OP_JLE;
            third->target_label = get_label();
            add_to_three_ac_list(list, third);

            generate_opcode(st_entry->data.block->body, st, list);

            three_ac *fourth = malloc(sizeof(three_ac));
            fourth->label = NULL;
            fourth->op = OP_INC;
            fourth->arg1 = loop_var_ste;
            fourth->arg2 = NULL;
            fourth->result = loop_var_ste;
            add_to_three_ac_list(list, fourth);

            three_ac *fifth = malloc(sizeof(three_ac));
            fifth->label = NULL;
            fifth->op = OP_JMP;
            fifth->target_label = first->label;
            add_to_three_ac_list(list, fifth);

            three_ac *sixth = malloc(sizeof(three_ac));
            sixth->label = third->target_label;
            sixth->op = OP_LABEL;
            add_to_three_ac_list(list, sixth);
        }
        else
        {
            three_ac *first = temp;
            first->label = get_label();
            first->op = OP_LABEL;
            add_to_three_ac_list(list, first);

            three_ac *second = malloc(sizeof(three_ac));
            second->label = NULL;
            second->op = OP_CMP;
            second->arg1 = loop_var_ste;
            second->arg2 = end_ste;
            add_to_three_ac_list(list, second);

            three_ac *third = malloc(sizeof(three_ac));
            third->label = NULL;
            third->op = OP_JGE;
            third->target_label = get_label();
            add_to_three_ac_list(list, third);

            generate_opcode(st_entry->data.block->body, st, list);

            three_ac *fourth = malloc(sizeof(three_ac));
            fourth->label = NULL;
            fourth->op = OP_DEC;
            fourth->arg1 = loop_var_ste;
            fourth->arg2 = NULL;
            fourth->result = loop_var_ste;
            add_to_three_ac_list(list, fourth);

            three_ac *fifth = malloc(sizeof(three_ac));
            fifth->label = NULL;
            fifth->op = OP_JMP;
            fifth->target_label = first->label;
            add_to_three_ac_list(list, fifth);

            three_ac *sixth = malloc(sizeof(three_ac));
            sixth->label = third->target_label;
            sixth->op = OP_LABEL;
            add_to_three_ac_list(list, sixth);
        }
        break;
    }
        /*
             code:
             switch (var)
                 case 1:
                     body1
                 case 2:
                     body2
                 default:
                     body3
             assembly:
             label0: cmp var, 1
                     jnz label1
                     body1
                     jmp endlabel
             label1: cmp var, 2
                     jnz label2
                     body2
                     jmp endlabel
             label2: body3
             endlabel:

         */
    case SWITCH_AST:
    {
        char *ste_name = malloc(sizeof(char) * 10);
        sprintf(ste_name, "%p", node);
        ST_ENTRY *ste = checkSymbolTable(st, ste_name);

        parse_tree_node *switch_var = node->left;
        ST_ENTRY *switch_var_ste = checkAllSymbolTables(st, switch_var->leafNodeInfo.lexeme);

        LinkedListASTNode *case_list = node->right;

        char *end_label = get_label();
        char *next_label;
        while (case_list != NULL)
        {
            ast_node *case_node = case_list->data;
            parse_tree_node *case_val = case_node->left;
            ast_node *case_body = case_node->right;

            if (case_val == NULL)
            {
                generate_opcode(case_body, st, list);

                three_ac *temp = malloc(sizeof(three_ac));
                temp->label = NULL;
                temp->op = OP_JMP;
                temp->target_label = end_label;
                add_to_three_ac_list(list, temp);
                break;
            }

            three_ac *second = malloc(sizeof(three_ac));
            second->label = NULL;
            second->op = OP_CMP;
            second->arg1 = switch_var_ste;
            second->arg2 = generate_opcode(case_val, st, list)->result;
            add_to_three_ac_list(list, second);

            three_ac *third = malloc(sizeof(three_ac));
            third->label = NULL;
            third->op = OP_JNZ;
            next_label = get_label();
            third->target_label = next_label;
            add_to_three_ac_list(list, third);

            generate_opcode(case_body, st, list);

            three_ac *fourth = malloc(sizeof(three_ac));
            fourth->label = NULL;
            fourth->op = OP_JMP;
            fourth->target_label = end_label;
            add_to_three_ac_list(list, fourth);

            three_ac *fifth = malloc(sizeof(three_ac));
            fifth->label = next_label;
            fifth->op = OP_LABEL;
            add_to_three_ac_list(list, fifth);
        }
        break;
    }
    default:
        break;
    }
}
#define TEMP_1 "t1"
#define TEMP_2 "t2"
#define TEMP_3 "t3"
#define TEMP_4 "t4"
#define TEMP_5 "t5"

void print_three_ac_list(three_ac_list *list, FILE *fp)
{
    fprintf(fp, "We're getting started \n");
    fflush(fp);
    three_ac *temp = list->head;
    while (temp != NULL)
    {
        switch (temp->op)
        {
        case OP_ADD:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg2->name);
            fprintf(fp, "add %s, %s\n", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_SUB:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg2->name);
            fprintf(fp, "sub %s, %s\n", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_MUL:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg2->name);
            fprintf(fp, "mul %s, %s\n", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_DIV:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg2->name);
            fprintf(fp, "div %s, %s\n", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_AND:
        case OP_OR:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg2->name);
            fprintf(fp, "%s %s, %s\n", temp->op == OP_AND ? "and" : "or", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_ASSIGN:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_CMP:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg2->name);
            fprintf(fp, "cmp %s, %s\n", TEMP_1, TEMP_2);
            break;
        case OP_JNZ:
            fprintf(fp, "jnz %s\n", temp->target_label);
            break;
        case OP_JMP:
            fprintf(fp, "jmp %s\n", temp->target_label);
            break;
        case OP_LABEL:
            fprintf(fp, "%s:\n", temp->label);
            break;
        case OP_DEC:
            fprintf(fp, "dec %s\n", temp->result->name);
            break;
        case OP_INC:
            fprintf(fp, "inc %s\n", temp->result->name);
            break;
        case OP_NEGATE:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "sub 0, %s\n", TEMP_1);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_SET:
            fprintf(fp, "SET\n");
            break;
        default:
            break;
        }
        temp = temp->next;
    }
}
int main()
{
    bufferSize = 1024;
    parseInputSourceCode("tests/t2.txt", "src/parser/parseTree.txt");
    printf("parse tree created successfully.\n");
    fflush(stdout);
    ast *AST = create_ast(&parseTree);
    printf("AST created successfully.\n");
    print_ast(AST);
    printf("AST printed successfully.\n");
    fflush(stdout);
    // populate symbol tables
    populateSymbolTables(AST);
    printf("Nah. No way.\n");
    fflush(stdout);

    // generate three address code
    three_ac_list *list3Ad = malloc(sizeof(three_ac_list));
    init_three_ac_list(list3Ad);
    if (AST->root == NULL)
        printf("AST->root is NULL\n");
    else
        printf("AST->root is NOT NULL\n");
    fflush(stdout);
    generate_opcode(AST->root, &symbolTable, list3Ad);
    printf("3AC generated successfully.\n");
    fflush(stdout);
    FILE *fp1 = fopen("3AC.txt", "w");
    print_three_ac_list(list3Ad, fp1);
    printf("3AC printed successfully.\n");
    fflush(stdout);
    return 0;
}