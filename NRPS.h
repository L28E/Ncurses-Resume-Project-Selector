#ifndef NRPS_H
#define NRPS_H
#include <stdbool.h>
#include <jansson.h>

char pre_itemize[] = "\\textbf{%s} \\hfill %s \\\\";

typedef struct
{
    char *title;
    bool selected;
} project;

int generate_tex(int num_projects, project *projects, json_t *projects_root);

#endif /* NRPS_H */
