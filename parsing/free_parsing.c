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
        if (current->redirection != NULL)
            free_redirection(current->redirection);
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
