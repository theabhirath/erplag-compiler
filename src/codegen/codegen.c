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
} OPERATOR;

struct three_ac {
    char *label;
    OPERATOR op;
    char *target_label;
    ST_ENTRY *arg1;
    ST_ENTRY *arg2;
    ST_ENTRY *result;
    struct three_ac *next;
}three_ac;

struct three_ac_list {
    struct three_ac *head;
    struct three_ac *tail;
}three_ac_list;

void add_to_three_ac_list(three_ac_list *list, three_ac *node)
{
    if(list->head == NULL)
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

ST_ENTRY *get_temp_var(symbol_table *symtab, enum TYPE type)
{
    ST_ENTRY *temp = malloc(sizeof(ST_ENTRY));
    temp->entry_type = VAR_SYM;
    temp->type = type;
    temp->offset = symtab->offset;
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
    const_true.data.var->type = __BOOL__;
    const_true->offset = 0;
    const_true->data.var->value = 1;

    const_false = malloc(sizeof(ST_ENTRY));
    const_false->entry_type = VAR_SYM;
    const_false.data.var->type = __BOOL__;
    const_false->offset = 0;
    const_false->data.var->value = 0;

}

enum TYPE getType(ast_node *node, symbol_table *st)
{
    switch(node->nodeType)
    {
        case PLUS_AST:
        case MINUS_AST:
        case MUL_AST:
        case DIV_AST:
        case AND_AST:
        case OR_AST:
        case LT_AST:
        case GT_AST:
        case LE_AST:
        case GE_AST:
        case EQ_AST:
        case NE_AST:
            return node->type;
        case NUM:
            return __NUM__;
        case RNUM:
            return __RNUM__;
        case TRUE_PTN_AST:
        case FALSE_PTN_AST:
            return __BOOL__;
        case ID:
        {
            ST_ENTRY *st_entry = checkAllSymbolTables(st, node->leafNodeInfo.lexeme);
            if(st_entry->entry_type == VAR_SYM)
                return st_entry->data.var->type;
            else if(st_entry->entry_type == ARR_SYM)
                return __ARRAY__;
            return __ERROR__;
        }
        case ARR_ELEM_AST:
        {
            ST_ENTRY *st_entry1 = checkAllSymbolTables(st, node->left->leafNodeInfo.lexeme);
            return st_entry1->data.arr->type;
        }
    }
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

    switch(node->nodeType)
    {
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
            temp->result = get_temp_var(st, getType(temp1->result));
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
                temp->result = get_temp_var(st, getType(temp1->result));
                add_to_three_ac_list(list, temp);
            }
            else
            {
                temp->op = OP_NEGATE;
                three_ac *temp1 = generate_opcode(node->right, st, list);
                temp->arg1 = temp1->result;
                temp->arg2 = NULL;
                temp->result = get_temp_var(st, getType(temp1->result));
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
            temp->result = get_temp_var(st, getType(temp1->result));
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
            temp->result = get_temp_var(st, getType(temp1->result));
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
            temp->result = get_temp_var(st, getType(temp1->result));
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
            temp->result = get_temp_var(st, getType(temp1->result));
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
        case EQ_AST: case NEQ_AST: case LT_AST: case GT_AST: case LTE_AST: case GTE_AST:
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
            switch(node->nodeType)
            {
                case EQ_AST:
                    second->op = OP_JZ;
                    break;
                case NEQ_AST:
                    second->op = OP_JNZ;
                    break;
                case LT_AST:
                    second->op = OP_JL;
                    break;
                case GT_AST:
                    second->op = OP_JG;
                    break;
                case LTE_AST:
                    second->op = OP_JLE;
                    break;
                case GTE_AST:
                    second->op = OP_JGE;
                    break;
            }
            second->target_label = get_label();
            add_to_three_ac_list(list, second);

            three_ac *third = malloc(sizeof(three_ac));
            third->label = NULL;
            third->Op = OP_RESET;
            third->arg1 = get_temp_var(st, TYPE_INT);
            third->arg2 = NULL;
            third->result = third->arg1;
            add_to_three_ac_list(list, third);

            three_ac *fourth = malloc(sizeof(three_ac));
            fourth->label = NULL;
            fourth->Op = OP_JMP;
            fourth->target_label = get_label();
            add_to_three_ac_list(list, fourth);

            three_ac *fifth = malloc(sizeof(three_ac));
            fifth->label = second->target_label;
            fifth->Op = OP_SET;
            fifth->arg1 = third->arg1;
            fifth->arg2 = NULL;
            fifth->result = third->arg1;
            add_to_three_ac_list(list, fifth);

            three_ac *sixth = malloc(sizeof(three_ac));
            sixth->label = fourth->target_label;
            sixth->Op = OP_LABEL;
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
            three_ac *first = malloc(sizeof(three_ac));
            first->label = get_label();
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
            label0:
                cmp ind, en
                jz label1
                body
                inc ind
                jmp label0
        */
       case FOR_AST:
       {
            char *name = malloc(sizeof(char) * 10);
            sprintf(name, "%p", node);
            ST_ENTRY *st_entry = checkSymbolTable(st, name);
            symbol_table *new_st = st_entry->data
       }

    }
}