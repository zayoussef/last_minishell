#include "../minishell.h"

void init_command(Command *cmd) 
{
    cmd->argv = (char **)malloc(MAX_TOKENS * sizeof(char *));
    cmd->redirection = NULL;
    cmd->heredoc = NULL;
    cmd->next = NULL;
}

Command* create_command() 
{
    Command *cmd = (Command *)malloc(sizeof(Command));
    if (!cmd) return NULL;
    init_command(cmd);
    return cmd;
}

void add_command_to_list(Command **head, Command *current) 
{
    if (*head == NULL) 
        *head = current;
    else 
    {
        Command *last = *head;
        while (last->next != NULL)
            last = last->next;
        last->next = current;
    }
}

void handle_word(Token *tokens, int *i, Command **current, int *argc) 
{
    if (*current == NULL)
    {
        *current = create_command();
        *argc = 0;
    }
    (*current)->argv[(*argc)++] = tokens[*i].value;
}
