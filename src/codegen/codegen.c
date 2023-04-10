#include <stdio.h>
#include "../ast/ast.h"
#include "../ast/symbol_table.h"

#define TEMP_1 "eax"
#define TEMP_2 "ebx"
#define TEMP_3 "ecx"
#define TEMP_4 "edx"

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
    OP_DEC,
    OP_SCOPE_START,
    OP_SCOPE_END,
    OP_TEMP_DECL,
    OP_FRAME_ALLOC,
    SCAN,
    PRINT_CONST,
    PRINT_VAR,
    INSTR,
    OP_NOP
} OPERATOR;

void print_op(OPERATOR op)
{
    switch (op)
    {
    case OP_ASSIGN:
        printf("OP_ASSIGN\n");
        break;
    case OP_ADD:
        printf("OP_ADD\n");
        break;
    case OP_SUB:
        printf("OP_SUB\n");
        break;
    case OP_MUL:
        printf("OP_MUL\n");
        break;
    case OP_DIV:
        printf("OP_DIV\n");
        break;
    case OP_AND:
        printf("OP_AND\n");
        break;
    case OP_OR:
        printf("OP_OR\n");
        break;
    case OP_JMP:
        printf("OP_JMP\n");
        break;
    case OP_JZ:
        printf("OP_JZ\n");
        break;
    case OP_JNZ:
        printf("OP_JNZ\n");
        break;
    case OP_NEGATE:
        printf("OP_NEGATE\n");
        break;
    case OP_SET:
        printf("OP_SET\n");
        break;
    case OP_RESET:
        printf("OP_RESET\n");
        break;
    case OP_CMP:
        printf("OP_CMP\n");
        break;
    case OP_LABEL:
        printf("OP_LABEL\n");
        break;
    case OP_JL:
        printf("OP_JL\n");
        break;
    case OP_JG:
        printf("OP_JG\n");
        break;
    case OP_JLE:
        printf("OP_JLE\n");
        break;
    case OP_JGE:
        printf("OP_JGE\n");
        break;
    case OP_INC:
        printf("OP_INC\n");
        break;
    case OP_DEC:
        printf("OP_DEC\n");
        break;
    case INSTR:
        printf("INSTR\n");
        break;
    case OP_SCOPE_START:
        printf("OP_SCOPE_START\n");
        break;
    case OP_SCOPE_END:
        printf("OP_SCOPE_END\n");
        break;
    case OP_TEMP_DECL:
        printf("OP_TEMP_DECL\n");
        break;
    case OP_FRAME_ALLOC:
        printf("OP_FRAME_ALLOC\n");
        break;
    case SCAN:
        printf("SCAN\n");
        break;
    case PRINT_CONST:
        printf("PRINT_CONST\n");
        break;
    case PRINT_VAR:
        printf("PRINT_VAR\n");
        break;
    case OP_NOP:
        printf("OP_NOP\n");
        break;
    default:
        printf("Unknown operator\n");
        break;
    }
}

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
}

ST_ENTRY *get_temp_var(symbol_table *symtab, enum TYPE type, three_ac_list *list)
{
    printf("Creating temp var\n");
    static int temp_vars = 0;
    ST_ENTRY *temp = malloc(sizeof(ST_ENTRY));
    temp->entry_type = VAR_SYM;
    temp->name = malloc(sizeof(char) * 64);
    sprintf(temp->name, "$temp_var%d", temp_vars);
    temp->data.var = malloc(sizeof(struct var_entry));
    temp->data.var->type = type;
    temp->data.var->offset = symtab->offset;
    symtab->offset += type == __NUM__ ? __NUM_SIZE__ : type == __RNUM__ ? __RNUM_SIZE__
                                                                        : __BOOL_SIZE__;
    temp_vars++;
    addToSymbolTable(symtab, temp);
    printf("Created temp var %s\n", temp->name);

    three_ac *node = malloc(sizeof(three_ac));
    node->label = NULL;
    node->op = OP_TEMP_DECL;
    node->target_label = NULL;
    node->arg1 = NULL;
    node->arg2 = NULL;
    node->result = temp;
    node->next = NULL;
    add_to_three_ac_list(list, node);
    return temp;
}
ST_ENTRY *const_true;
ST_ENTRY *const_false;

void init_three_ac_list(three_ac_list *list)
{
    list->head = NULL;
    list->tail = NULL;

    const_true = malloc(sizeof(ST_ENTRY));
    const_true->name = "dword[cnst_true]";
    const_true->entry_type = VAR_SYM;
    const_true->data.var = malloc(sizeof(struct var_entry));
    const_true->data.var->type = __BOOL__;
    const_true->data.var->offset = 0;
    const_true->data.var->val.boolValue = 1;

    const_false = malloc(sizeof(ST_ENTRY));
    const_false->name = "dword[cnst_false]";
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

three_ac *new_3ac()
{
    three_ac *temp = malloc(sizeof(three_ac));
    temp->label = NULL;
    temp->arg1 = NULL;
    temp->arg2 = NULL;
    temp->result = NULL;
    temp->next = NULL;
    temp->target_label = NULL;
    return temp;
}

ST_ENTRY *get_offset_with_width(ST_ENTRY *entry)
{
    ST_ENTRY *temp = malloc(sizeof(ST_ENTRY));
    temp->entry_type = VAR_SYM;
    temp->name = malloc(sizeof(char) * 64);
    sprintf(temp->name, "dword[rbp-%d]", entry->data.var->offset);
    return temp;
}

int get_offset(ST_ENTRY *entry)
{
    return entry->data.var->offset;
}

three_ac *generate_opcode(ast_node *node, symbol_table *st, three_ac_list *list)
{
    three_ac *temp = new_3ac();

    switch (node->nodeType)
    {
    case ID:
    {
        printf("Generating code for ID \n");
        temp->result = get_offset_with_width(checkAllSymbolTables(st, getName(node)));
        printf("Lexeme %s \n", temp->result->name);
        break;
    }
    case NUM:
    {
        printf("Generating code for NUM \n");
        temp->result = malloc(sizeof(ST_ENTRY));
        temp->result->entry_type = VAR_SYM;
        temp->result->name = getName(node);
        printf("Lexeme %s \n", temp->result->name);
        break;
    }
    case RNUM:
    {
        printf("Generating code for RNUM \n");
        temp->result = malloc(sizeof(ST_ENTRY));
        temp->result->name = getName(node);
        printf("Lexeme %s \n", temp->result->name);
        break;
    }
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
        three_ac *temp1 = new_3ac();
        temp1->op = OP_LABEL;
        temp1->label = "main";
        add_to_three_ac_list(list, temp1);
        temp->op = OP_SCOPE_START;
        add_to_three_ac_list(list, temp);
        // Frame alloc
        temp = new_3ac();
        temp->op = OP_FRAME_ALLOC;
        temp->arg1 = driver_st_entry;
        add_to_three_ac_list(list, temp);
        while (driver_list != NULL)
        {
            printf("Statement \n");
            generate_opcode(driver_list->data, driver_st, list);
            driver_list = driver_list->next;
        }
        temp = new_3ac();
        temp->op = OP_SCOPE_END;
        add_to_three_ac_list(list, temp);

        printf("Reached end of driver module \n");
        break;
    }
    case EQUALS_AST:
    {
        printf("Generating code for assignment \n");
        temp->op = OP_ASSIGN;
        three_ac *temp1 = generate_opcode(node->right, st, list);
        temp->arg1 = temp1->result;
        temp->arg2 = NULL;
        temp->result = generate_opcode(node->left, st, list)->result;
        add_to_three_ac_list(list, temp);
        printf("Generated code for assignment \n");
        break;
    }
    case PLUS_AST:
    {
        printf("Generating code for plus \n");
        temp->op = OP_ADD;
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);
        printf("Args %s %s \n", temp1->result->name, temp2->result->name);
        fflush(stdout);
        temp->arg1 = temp1->result;
        temp->arg2 = temp2->result;
        temp->result = get_offset_with_width(get_temp_var(st, getType(temp1->result, st), list));
        add_to_three_ac_list(list, temp);
        break;
    }
    case MINUS_AST:
    {
        if (node->left != NULL)
        {
            printf("Generating code for minus \n");
            temp->op = OP_SUB;
            three_ac *temp1 = generate_opcode(node->left, st, list);
            three_ac *temp2 = generate_opcode(node->right, st, list);
            printf("Args %s %s \n", temp1->result->name, temp2->result->name);
            temp->arg1 = temp1->result;
            temp->arg2 = temp2->result;
            temp->result = get_offset_with_width(get_temp_var(st, getType(temp1->result, st), list));
            add_to_three_ac_list(list, temp);
        }
        else
        {
            printf("Generating code for unary minus \n");
            temp->op = OP_NEGATE;
            three_ac *temp1 = generate_opcode(node->right, st, list);
            temp->arg1 = temp1->result;
            temp->arg2 = NULL;
            temp->result = get_offset_with_width(get_temp_var(st, getType(temp1->result, st), list));
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
        temp->result = get_offset_with_width(get_temp_var(st, getType(temp1->result, st), list));
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
        temp->result = get_offset_with_width(get_temp_var(st, getType(temp1->result, st), list));
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
        temp->result = get_offset_with_width(get_temp_var(st, getType(temp1->result, st), list));
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
        temp->result = get_offset_with_width(get_temp_var(st, getType(temp1->result, st), list));
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
        printf("Generating code for relational expression \n");
        three_ac *temp1 = generate_opcode(node->left, st, list);
        three_ac *temp2 = generate_opcode(node->right, st, list);
        printf("Obtained two operands: %s and %s\n", temp1->result->name, temp2->result->name);
        three_ac *first = temp;
        first->op = OP_CMP;
        first->arg1 = temp1->result;
        first->arg2 = temp2->result;
        first->result = NULL;
        add_to_three_ac_list(list, first);
        printf("Added cmp instruction\n");

        three_ac *second = new_3ac();
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
        printf("Added a conditional jump instruction\n");
        fflush(stdout);

        three_ac *third = new_3ac();
        third->op = OP_RESET;
        third->arg1 = get_offset_with_width(get_temp_var(st, __BOOL__, list));
        printf("Created a temp variable %s\n", third->arg1->name);
        third->result = third->arg1;
        add_to_three_ac_list(list, third);
        printf("Added a reset instruction");
        fflush(stdout);

        three_ac *fourth = new_3ac();
        fourth->op = OP_JMP;
        fourth->target_label = get_label();
        add_to_three_ac_list(list, fourth);
        printf("Added a jump instruction");

        three_ac *fifth = new_3ac();
        fifth->label = second->target_label;
        fifth->op = OP_LABEL;
        add_to_three_ac_list(list, fifth);
        printf("Added a label instruction");

        three_ac *sixth = new_3ac();
        sixth->op = OP_SET;
        sixth->arg1 = third->arg1;
        sixth->result = third->arg1;
        add_to_three_ac_list(list, sixth);
        printf("Added a set instruction");

        three_ac *seventh = new_3ac();
        seventh->label = fourth->target_label;
        seventh->op = OP_LABEL;
        add_to_three_ac_list(list, seventh);
        printf("Added a label instruction");

        return third;
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
        printf("Added label\n");

        three_ac *second = new_3ac();
        second->op = OP_CMP;
        if (node->left->nodeType == ID)
        {
            printf("Name: %s\n", getName(node->left));
            second->arg1 = generate_opcode(node->left, st, list)->result;
        }
        else
        {
            second->arg1 = generate_opcode(node->left, st, list)->result;
            printf("In else\n");
        }
        second->arg2 = const_false;
        add_to_three_ac_list(list, second);

        printf("Added cmp\n");
        three_ac *third = new_3ac();
        third->op = OP_JZ;
        third->target_label = get_label();
        add_to_three_ac_list(list, third);

        printf("Added jz\n");
        LinkedListASTNode *body = node->right;
        printf("Body: %p\n", body);
        char *symName = malloc(sizeof(char) * 100);
        sprintf(symName, "%p", node);
        ST_ENTRY *body_ste = checkAllSymbolTables(st, symName);
        printf("Body ste: %p\n", body_ste);

        // Scope start
        three_ac *scope_start = new_3ac();
        scope_start->op = OP_SCOPE_START;
        add_to_three_ac_list(list, scope_start);
        // Frame allocation
        three_ac *frame = new_3ac();
        frame->op = OP_FRAME_ALLOC;
        frame->arg1 = body_ste;
        add_to_three_ac_list(list, frame);

        while (body != NULL)
        {
            generate_opcode(body->data, body_ste->data.block->symTable, list);
            body = body->next;
        }
        // Scope end
        three_ac *scope_end = new_3ac();
        scope_end->op = OP_SCOPE_END;
        add_to_three_ac_list(list, scope_end);

        printf("Added body\n");
        three_ac *fourth = new_3ac();
        fourth->op = OP_JMP;
        fourth->target_label = first->label;
        add_to_three_ac_list(list, fourth);

        three_ac *fifth = new_3ac();
        fifth->label = third->target_label;
        fifth->op = OP_LABEL;
        add_to_three_ac_list(list, fifth);
        break;
    }
    case TRUE_PTN_AST:
    {
        temp->result = const_true;
        break;
    }
    case FALSE_PTN_AST:
    {
        temp->result = const_false;
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

        // scope start
        three_ac *scope_start = new_3ac();
        scope_start->op = OP_SCOPE_START;
        add_to_three_ac_list(list, scope_start);
        // frame allocation
        three_ac *frame = new_3ac();
        frame->op = OP_FRAME_ALLOC;
        frame->arg1 = st_entry;
        add_to_three_ac_list(list, frame);
        if (start < end)
        {
            three_ac *first = temp;
            first->label = get_label();
            first->op = OP_LABEL;
            add_to_three_ac_list(list, first);

            three_ac *second = new_3ac();
            second->op = OP_CMP;
            second->arg1 = get_offset_with_width(loop_var_ste);
            second->arg2 = get_offset_with_width(end_ste);
            add_to_three_ac_list(list, second);

            three_ac *third = new_3ac();
            third->op = OP_JLE;
            third->target_label = get_label();
            add_to_three_ac_list(list, third);

            LinkedListASTNode *body = st_entry->data.block->body;
            while (body != NULL)
            {
                generate_opcode(body->data, new_st, list);
                body = body->next;
            }

            three_ac *fourth = new_3ac();
            fourth->op = OP_INC;
            fourth->arg1 = second->arg1;
            fourth->arg2 = NULL;
            fourth->result = second->arg1;
            add_to_three_ac_list(list, fourth);

            three_ac *fifth = new_3ac();
            fifth->op = OP_JMP;
            fifth->target_label = first->label;
            add_to_three_ac_list(list, fifth);

            three_ac *sixth = new_3ac();
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

            three_ac *second = new_3ac();
            second->op = OP_CMP;
            second->arg1 = get_offset_with_width(loop_var_ste);
            second->arg2 = get_offset_with_width(end_ste);
            add_to_three_ac_list(list, second);

            three_ac *third = new_3ac();
            third->op = OP_JGE;
            third->target_label = get_label();
            add_to_three_ac_list(list, third);

            LinkedListASTNode *body = st_entry->data.block->body;
            while (body != NULL)
            {
                generate_opcode(body->data, new_st, list);
                body = body->next;
            }
            printf("Added body\n");

            three_ac *fourth = new_3ac();
            fourth->op = OP_DEC;
            fourth->arg1 = second->arg1;
            fourth->arg2 = NULL;
            fourth->result = second->arg1;
            add_to_three_ac_list(list, fourth);

            three_ac *fifth = new_3ac();
            fifth->op = OP_JMP;
            fifth->target_label = first->label;
            add_to_three_ac_list(list, fifth);

            three_ac *sixth = new_3ac();
            sixth->label = third->target_label;
            sixth->op = OP_LABEL;
            add_to_three_ac_list(list, sixth);
        }
        // scope end
        three_ac *scope_end = new_3ac();
        scope_end->op = OP_SCOPE_END;
        add_to_three_ac_list(list, scope_end);

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
        parse_tree_node *switch_var = node->left;
        ST_ENTRY *switch_var_ste = checkAllSymbolTables(st, switch_var->leafNodeInfo.lexeme);

        LinkedListASTNode *case_list = node->right;

        char *end_label = get_label();
        char *next_label;
        while (case_list != NULL)
        {
            ast_node *case_node = case_list->data;
            if (case_node == NULL)
            {
                break;
            }
            parse_tree_node *case_val = case_node->left;
            ast_node *case_body = case_node->right;

            char *st_name = malloc(sizeof(char) * 16);
            sprintf(st_name, "%p", case_node);
            ST_ENTRY *st_entry = checkSymbolTable(st, st_name);
            symbol_table *case_st = st_entry->data.block->symTable;

            if (case_val == NULL)
            {
                LinkedListASTNode *body = case_body;
                // scope start
                three_ac *scope_start = new_3ac();
                scope_start->op = OP_SCOPE_START;
                add_to_three_ac_list(list, scope_start);

                // frame allocation
                three_ac *frame_alloc = new_3ac();
                frame_alloc->op = OP_FRAME_ALLOC;
                frame_alloc->arg1 = st_entry;
                add_to_three_ac_list(list, frame_alloc);
                while (body != NULL)
                {
                    generate_opcode(body->data, case_st, list);
                    body = body->next;
                }
                // scope end
                three_ac *scope_end = new_3ac();
                scope_end->op = OP_SCOPE_END;
                add_to_three_ac_list(list, scope_end);
                printf("Added default body\n");

                temp->op = OP_JMP;
                temp->target_label = end_label;
                add_to_three_ac_list(list, temp);
                case_list = case_list->next;
                printf("Completed default case\n");
                break;
            }

            three_ac *second = temp;
            second->op = OP_CMP;
            second->arg1 = get_offset_with_width(switch_var_ste);
            second->arg2 = generate_opcode(case_val, st, list)->result;
            add_to_three_ac_list(list, second);
            printf("Added compare\n");

            three_ac *third = new_3ac();
            third->op = OP_JNZ;
            next_label = get_label();
            third->target_label = next_label;
            printf("Added jnz\n");

            LinkedListASTNode *body = st_entry->data.block->body;
            // scope start
            three_ac *scope_start = new_3ac();
            scope_start->op = OP_SCOPE_START;
            add_to_three_ac_list(list, scope_start);

            // frame allocation
            three_ac *frame_alloc = new_3ac();
            frame_alloc->op = OP_FRAME_ALLOC;
            frame_alloc->arg1 = st_entry;
            while (body != NULL)
            {
                generate_opcode(body->data, case_st, list);
                body = body->next;
                printf("Statement in case body generated\n");
            }
            // scope end
            three_ac *scope_end = new_3ac();
            scope_end->op = OP_SCOPE_END;
            add_to_three_ac_list(list, scope_end);

            three_ac *fourth = new_3ac();
            fourth->op = OP_JMP;
            fourth->target_label = end_label;
            add_to_three_ac_list(list, fourth);
            printf("Added jmp\n");

            three_ac *fifth = new_3ac();
            fifth->label = next_label;
            fifth->op = OP_LABEL;
            add_to_three_ac_list(list, fifth);
            printf("Added label\n");

            case_list = case_list->next;
        }
        three_ac *sixth = new_3ac();
        sixth->label = end_label;
        sixth->op = OP_LABEL;
        add_to_three_ac_list(list, sixth);
        printf("Added end label\n");

        break;
    }
    case GET_VALUE_AST:
    {
        printf("Generating code for get_value\n");

        ST_ENTRY *ste = checkAllSymbolTables(st, getName(node->right));
        temp->op = SCAN;
        temp->result = ste;
        add_to_three_ac_list(list, temp);
        break;
    }
    case PRINT_AST:
    {
        switch (node->right->nodeType)
        {
        case ID:
        {
            ST_ENTRY *ste = checkAllSymbolTables(st, getName(node->right));
            temp->op = PRINT_VAR;
            temp->arg1 = ste;
            break;
        }
        case ARR_ELEM_AST:
        {
            ST_ENTRY *ste = checkAllSymbolTables(st, getName(node->right->left));
            temp->op = OP_NOP;
            break;
        }
        default:
        {
            temp->op = PRINT;
            temp->arg1 = generate_opcode(node->right, st, list)->result;
            break;
        }
        }
        add_to_three_ac_list(list, temp);
        break;
    }

    default:
        printf("Error: Unknown AST node type %d\n", node->nodeType);
        break;
    }
    return temp;
}

void print_three_ac_list(three_ac_list *list, FILE *fp)
{
    three_ac *temp = list->head;
    while (temp != NULL)
    {
        print_op(temp->op);
        switch (temp->op)
        {
        case OP_ADD:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_2, temp->arg2->name);
            fprintf(fp, "add %s, %s\n", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_SUB:

            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_2, temp->arg2->name);
            fprintf(fp, "sub %s, %s\n", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_MUL:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_2, temp->arg2->name);
            fprintf(fp, "mul %s\n", TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_DIV:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_2, temp->arg2->name);
            fprintf(fp, "div %s, %s\n", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_AND:
        case OP_OR:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_2, temp->arg2->name);
            fprintf(fp, "%s %s, %s\n", temp->op == OP_AND ? "and" : "or", TEMP_1, TEMP_2);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_ASSIGN:
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", temp->result->name, TEMP_1);
            break;
        case OP_CMP:
            printf("In CMP\n");
            fflush(stdout);
            if (temp->arg1 == NULL || temp->arg2 == NULL)
            {
                printf("arg1: %p\n", temp->arg1);
                fflush(stdout);
                printf("arg2: %p\n", temp->arg2);
                fflush(stdout);
            }
            printf("arg1: %s\n", temp->arg1->name);
            fflush(stdout);
            printf("arg2: %s\n", temp->arg2->name);
            fflush(stdout);
            fprintf(fp, "mov %s, %s\n", TEMP_1, temp->arg1->name);
            fprintf(fp, "mov %s, %s\n", TEMP_2, temp->arg2->name);
            fprintf(fp, "cmp %s, %s\n", TEMP_1, TEMP_2);
            break;
        case OP_JNZ:
            fprintf(fp, "jnz %s\n", temp->target_label);
            break;
        case OP_JMP:
            fprintf(fp, "jmp %s\n", temp->target_label);
            break;
        case OP_JZ:
            fprintf(fp, "jz %s\n", temp->target_label);
            break;
        case OP_JG:
            fprintf(fp, "jg %s\n", temp->target_label);
            break;
        case OP_JL:
            fprintf(fp, "jl %s\n", temp->target_label);
            break;
        case OP_JGE:
            fprintf(fp, "jge %s\n", temp->target_label);
            break;
        case OP_JLE:
            fprintf(fp, "jle %s\n", temp->target_label);
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
            fprintf(fp, "mov %s, 1\n", temp->arg1->name);
            break;
        case OP_RESET:
            fprintf(fp, "mov %s, 0\n", temp->arg1->name);
            break;
        case OP_SCOPE_START:
            fprintf(fp, "push rbp\n");
            fprintf(fp, "mov rbp, rsp\n");
            break;
        case OP_SCOPE_END:
            fprintf(fp, "mov rsp, rbp\n");
            fprintf(fp, "pop rbp\n");
            break;
        case OP_TEMP_DECL:
            // Decrement the stack pointer by the size of the variable
            // Size depends on the type of the variable
            // if (temp->result->entry_type == VAR_SYM)
            //     fprintf(fp, "sub rsp, %d\n", temp->result->data.var->type == __NUM__ ? __NUM_SIZE__ : temp->result->data.var->type == __BOOL__ ? __BOOL_SIZE__ : __RNUM_SIZE__);
            // else if (temp->result->entry_type == ARR_SYM)
            // {
            //     if (temp->result->data.arr->arrayType == STATIC)
            //     {
            //         fprintf(fp, "sub rsp, %d\n", temp->result->data.arr->eltype == __NUM__ ? __NUM_SIZE__ * (temp->result->data.arr->right.staticIndex - temp->result->data.arr->left.staticIndex + 1) : temp->result->data.arr->eltype == __BOOL__ ? __BOOL_SIZE__ * (temp->result->data.arr->right.staticIndex - temp->result->data.arr->left.staticIndex + 1) : __RNUM_SIZE__ * (temp->result->data.arr->right.staticIndex - temp->result->data.arr->left.staticIndex + 1));
            //     }
            // }
            break;
        case OP_FRAME_ALLOC:
            ST_ENTRY *temp1 = temp->arg1;
            int offset = 0;
            if (temp1->entry_type == FUNC_SYM)
            {
                offset = temp1->data.func->symTable->offset;
            }
            else if (temp1->entry_type == BLOCK_SYM)
            {
                offset = temp1->data.block->symTable->offset;
            }
            fprintf(fp, "sub rsp, %d\n", offset);
            break;
        case OP_NOP:
            fprintf(fp, "nop\n");
            break;
        // TODO: Scan and Print
        case SCAN:
        {
            if (temp->result->entry_type == VAR_SYM)
            {
                if (temp->result->data.var->type == __NUM__)
                {
                    // print prompt
                    // align the stack to 16 bytes
                    fprintf(fp, "sub rsp, 8\n");
                    fprintf(fp, "and rsp, -16\n");
                    // call printf
                    fprintf(fp, "mov rdi, prompt_num\n");
                    fprintf(fp, "mov rax, 0\n");
                    fprintf(fp, "call printf\n");
                    // align the stack to 16 bytes
                    fprintf(fp, "sub rsp, 8\n");
                    fprintf(fp, "and rsp, -16\n");
                    // call scanf
                    fprintf(fp, "mov rdi, scan_num\n");
                    fprintf(fp, "mov rsi, rbp\n");
                    fprintf(fp, "sub rsi, %d\n", get_offset(temp->result));
                    fprintf(fp, "mov rax, 0\n");
                    fprintf(fp, "call scanf\n");

                }
                else if (temp->result->data.var->type == __RNUM__)
                {
                }
                else if (temp->result->data.var->type == __BOOL__)
                {
                }
                else
                {
                }
            }
            break;
        }
        case PRINT_CONST:
        case PRINT_VAR:
        {
            if (temp->arg1->entry_type == VAR_SYM)
            {
                if (temp->arg1->data.var->type == __NUM__)
                {
                    // align the stack pointer to 16 bytes
                    fprintf(fp, "sub rsp, 8\n");
                    fprintf(fp, "and rsp, -16\n");
                    // call printf
                    fprintf(fp, "mov rdi, print_num\n");
                    fprintf(fp, "movsx rsi, %s\n", get_offset_with_width(temp->arg1)->name);
                    fprintf(fp, "mov rax, 0\n");
                    fprintf(fp, "call printf\n");


                }
                else if (temp->arg1->data.var->type == __RNUM__)
                {
                }
                /*
                    mov TEMP_1, get_offset_with_width(temp->arg1)
                    dec TEMP_1
                    jnz l1
                    mov rdi, false_str
                    jmp l2
                l1:
                    mov rdi, true_str
                l2:
                    mov rax, 0
                    call printf
                */
                else if (temp->arg1->data.var->type == __BOOL__)
                {
                    // // save the stack pointer
                    // fprintf(fp, "mov r8, rsp\n");
                    // align the stack pointer to 16 bytes
                    fprintf(fp, "sub rsp, 8\n");
                    fprintf(fp, "and rsp, -16\n");
                    // call printf
                    fprintf(fp, "mov %s, %s\n", TEMP_1, get_offset_with_width(temp->arg1)->name);
                    fprintf(fp, "sub %s, 1\n", TEMP_1);
                    char *l1 = get_label();
                    char *l2 = get_label();
                    fprintf(fp, "jz %s\n", l1);
                    fprintf(fp, "mov rdi, false_str\n");
                    fprintf(fp, "jmp %s\n", l2);
                    fprintf(fp, "%s:\n", l1);
                    fprintf(fp, "mov rdi, true_str\n");
                    fprintf(fp, "%s:\n", l2);
                    fprintf(fp, "mov rax, 0\n");
                    fprintf(fp, "call printf\n");

                    // // restore the stack pointer
                    // fprintf(fp, "mov rsp, r8\n");

                }
                else
                {
                }
            }
            break;
        }
        default:
            printf("Unknown opcode %d\n", temp->op);
            break;
        }
        fprintf(fp, "\n");
        fflush(fp);
        fflush(stdout);
        temp = temp->next;
    }
    printf("Reached end of list\n");
    fflush(stdout);
}

void print_init(FILE *fp)
{
    fprintf(fp, "extern printf\n");
    fprintf(fp, "extern scanf\n");
    fprintf(fp, "global main\n");
    fprintf(fp, "section .data\n");
    fprintf(fp, "cnst_true dd 1\n");
    fprintf(fp, "cnst_false dd 0\n");
    // Scanf format strings
    fprintf(fp, "scan_num db \"%%d\", 0\n");
    // Print format strings
    fprintf(fp, "print_num db \"Output: %%d\", 10, 0\n");
    // bools
    fprintf(fp, "true_str db \"Output: true\", 10, 0\n");
    fprintf(fp, "false_str db \"Output: false\", 10, 0\n");
    // prompts
    fprintf(fp, "prompt_num db \"Input: Enter an integer value\", 10, 0\n");
    fprintf(fp, "prompt_rnum db \"Input: Enter a real value\", 10, 0\n");
    fprintf(fp, "prompt_bool db \"Input: Enter a boolean value\", 10, 0\n");

    fprintf(fp, "\n\nsection .text\n");
}

void print_fine(FILE *fp)
{
    fprintf(fp, "mov rax, 60\n");
    fprintf(fp, "mov rdi, 0\n");
    fprintf(fp, "syscall\n");
}

int main()
{
    bufferSize = 1024;
    parseInputSourceCode("tests/codegen/c1.txt", "src/parser/parseTree.txt");
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
    FILE *fp1 = fopen("code.asm", "w");
    print_init(fp1);
    print_three_ac_list(list3Ad, fp1);
    print_fine(fp1);
    printf("3AC printed successfully.\n");
    fflush(stdout);
    return 0;
}
