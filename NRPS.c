#include "NRPS.h"

#include <ncurses.h>

#include <string.h>
#include <stdio.h>

int main()
{
    int key;
    int current = 0;
    int max_rows = 15;
    json_t *projects_root;
    json_error_t error;
    project *projects;

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    // Load the JSON array
    projects_root = json_load_file("projects.json", 0, &error); // TODO: Allow user to provide a filename
    if (!projects_root)
    {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        endwin();
        return 1;
    }

    if (!json_is_array(projects_root))
    {                                  
        fprintf(stderr, "error: root is not an array\n");
        json_decref(projects_root);
        endwin();
        return 1;
    }

    // Loop through the array and store the projects.
    int num_projects = json_array_size(projects_root);
    projects = malloc(sizeof(project) * num_projects);

    for (int i = 0; i < num_projects; i++)
    {
        json_t *project, *title;

        project = json_array_get(projects_root, i);
        json_unpack_ex(project, &error, JSON_STRICT, "{s:s,s:s,s:[*]}", "title", &projects[i].title);
        if (!projects[i].title)
        {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            endwin();
            return 1;
        }
        projects[i].selected = FALSE;
    }

    // TUI
    int current_project_y = 4;  // The row on which the current project is displayed
    int i, j;
    while (key != 'q')
    {
        move(max_rows + 3, 0);
        printw("Up and Down to navigate, Space to toggle, E to export, Q to quit");

        // Print the current project.
        move(current_project_y, 0);
        if (projects[current].selected == TRUE)
        {
            wattr_on(stdscr, A_REVERSE, NULL);
            printw(">   %s", projects[current].title);
            wattr_off(stdscr, A_REVERSE, NULL);
        }
        else
        {
            printw(">   %s", projects[current].title);
        }

        // Print some preceeding projects.
        j = 0;
        for (i = current - 1; i >= 0 && current_project_y - 1 - j >= 0; i--)
        {
            move(current_project_y - 1 - j, 0);
            j++;
            if (projects[i].selected)
            {
                wattr_on(stdscr, A_REVERSE, NULL);
                printw("  %s", projects[i].title);
                wattr_off(stdscr, A_REVERSE, NULL);
            }
            else
            {
                printw("  %s", projects[i].title);
            }
        }

        // Print some proceeding projects.
        j = 0;
        for (i = current + 1; i < num_projects && current_project_y + 1 + j <= max_rows; i++)
        {
            move(current_project_y + 1 + j, 0);
            j++;
            if (projects[i].selected)
            {
                wattr_on(stdscr, A_REVERSE, NULL);
                printw("  %s", projects[i].title);
                wattr_off(stdscr, A_REVERSE, NULL);
            }
            else
            {
                printw("  %s", projects[i].title);
            }
        }

        key = getch();
        clear();

        switch (key)
        {
        case KEY_UP:
            if (current != 0)
                current--;
            break;
        case KEY_DOWN:
            if (current != num_projects - 1)
                current++;
            break;
        case ' ':
            projects[current].selected = !projects[current].selected;
            break;
        case 'e':
            move(max_rows + 4, 0);
            printw("Exported!");
            generate_tex(num_projects, projects, projects_root);
            break;
        default:
            break;
        }
    }

    endwin();
    return 0;
}

int generate_tex(int num_projects, project *projects, json_t *projects_root)
{
    char *dates;
    char *title;
    json_t *items;

    FILE *file = fopen("projects.tex", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    for (int i = 0; i < num_projects; i++)
    {
        if (projects[i].selected)
        {
            json_unpack(json_array_get(projects_root, i), "{s:s,s:s,s:o}", "title", &title, "date_range", &dates, "items", &items);

            fprintf(file, pre_itemize, title, dates);
            fprintf(file, "\n\\begin{itemize}\n");
            for (int j = 0; j < json_array_size(items); j++)
            {
                fprintf(file, "\\item %s\n", json_string_value(json_array_get(items, j)));
            }
            fprintf(file, "\\end{itemize}\n\n");
        }
    }
    fclose(file);
    return 0;
}