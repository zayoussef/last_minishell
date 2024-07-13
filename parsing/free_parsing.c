#include "../minishell.h"

void free_redirection(Redirection *redir) 
{
    while (redir != NULL) 
    {
        Redirection *next = redir->next;
        if (redir->filename != NULL)
            free(redir->filename);
        free(redir);
        redir = next;
    }
}

void free_all_v2(Command *current)
{
        if (current->input != NULL)
            free_redirection(current->input);
        if (current->output != NULL)
            free_redirection(current->output);
        if (current->append_output != NULL) 
            free_redirection(current->append_output);
        if (current->heredoc != NULL)
            free_redirection(current->heredoc);
}

void free_all_resources(Command *head)
 {
    Command *current;
    Command *next;
    current = head;
    int i;
    if (head == NULL)
        return;
    while (current != NULL) {
        next = current->next;
        if (current->argv != NULL) {
            i = 0;
            while (current->argv[i] != NULL) 
            {
                if (current->argv[i] != NULL)
                    free(current->argv[i]);
                i++;
            }
            if (current->argv != NULL)
                free(current->argv);
        }
        free_all_v2(current);
        if (current != NULL)
            free(current);
        current = next;
    }
}


void print_commands(Command *head) {
    if (!head) {
        printf("No commands found\n");
        return;
    }
    int i;
    Command *cmd = head; 
    while(cmd != NULL) {
        printf("Command:\n");
        i = 0;
        while(cmd->argv[i] != NULL) {
            printf(" argv[%d]: %s\n", i, cmd->argv[i]);
            i++;
        }
        while(cmd->input) {
            printf(" Input redirection: %s\n", cmd->input->filename);
            cmd->input = cmd->input->next;
        }
        while(cmd->output) {
            printf(" Output redirection: %s\n", cmd->output->filename);
            cmd->output = cmd->output->next;
        }
        while (cmd->append_output) {
            printf(" Append output redirection: %s\n", cmd->append_output->filename);
            cmd->append_output = cmd->append_output->next;
        }
        while(cmd->heredoc) {
            printf(" Here-doc redirection: %s\n", cmd->heredoc->filename);
            cmd->heredoc = cmd->heredoc->next;
        }
        cmd = cmd->next;
    }
}