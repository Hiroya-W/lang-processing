#include "mppl_compiler.h"

#define INDENT_SIZE_NAME 20
#define INDENT_SIZE_TYPE 30
#define INDENT_SIZE_DEF 4

struct ID *globalidroot, *localidroot; /*! Pointers to root of global only & local only */

/*! Pointers to root of global + local symbol tables */
struct ID *crtabroot;

/*! Pointers to root of id symbol tables without type */
struct ID *id_without_type_root;

/*! Release the struct ID */
static void free_strcut_ID(struct ID **root);

/*! Release the struct TYPE */
static void free_struct_TYPE(struct TYPE *root);

/*! search the name pointed by name */
static struct ID *search_tab(struct ID **root, char *name, char *procname);

/*! To set the procedure name */
void set_procedure_name(char *name);

/*! Register the name pointed by name root */
static int id_register_to_tab(struct ID **root, char *name, char *procname, struct TYPE **type, int ispara, int deflinenum);

static int add_type_to_parameter_list(struct ID **root, char *procname, struct TYPE **type);

/*! the procedure name currenty being parsed */
static char current_procedure_name[MAXSTRSIZE];

static int add_id_to_crtab(struct ID *root);

/*! To set the procedure name */
void set_procedure_name(char *name) {
    strncpy(current_procedure_name, name, MAXSTRSIZE);
}

/*!
 * @brief Initialise the table
 */
void init_crtab() {
    globalidroot = NULL;
    localidroot = NULL;
    crtabroot = NULL;
    id_without_type_root = NULL;
    return;
}

/*!
 * @brief search the name pointed by name and procname
 * @param[in] root The root of the id-list
 * @param[in] name Name you want to find
 * @param[in] procname procedure name you want to find
 * @return struct TYPE * Return a pointer to the structure with matching name. 
 */
static struct ID *search_tab(struct ID **root, char *name, char *procname) {
    struct ID *p;

    for (p = *root; p != NULL; p = p->nextp) {
        if (strcmp(name, p->name) == 0) {
            /* when name and p->name are globalid(= procname and p->procname are NULL) */
            if (procname == NULL && p->procname == NULL) {
                return (p);
            }
            /* when name and p->name are localid(= procname and p->procname are not NULL) */
            else if (procname != NULL && p->procname != NULL && strcmp(procname, p->procname) == 0) {
                return (p);
            }
        }
    }
    return (NULL);
}

/*!
 * @brief Register the name pointed by name global or local
 * @param[in] type type for a name to be registered
 * @return int Return 0 on success and -1 on failure.
 */
int id_register_as_type(struct TYPE **type) {
    int ret, ret1;
    struct ID *p;

    if (type == NULL) {
        return error("struct TYPE is NULL\n");
    }

    for (p = id_without_type_root; p != NULL; p = p->nextp) {
        char *name = p->name;
        char *current_procedure_name = p->procname;
        int ispara = p->ispara;
        int deflinenum = p->deflinenum;
        if (definition_procedure_name) {
            /* regist procedure name */
            ret = id_register_to_tab(&globalidroot, name, NULL, type, ispara, deflinenum);
            ret1 = id_register_to_tab(&crtabroot, name, NULL, type, ispara, deflinenum);
        } else if (in_subprogram_declaration) {
            /* regist local name and formal parameter */
            ret = id_register_to_tab(&localidroot, name, current_procedure_name, type, ispara, deflinenum);
            ret1 = id_register_to_tab(&crtabroot, name, current_procedure_name, type, ispara, deflinenum);
        } else {
            /* regist global name */
            ret = id_register_to_tab(&globalidroot, name, NULL, type, ispara, deflinenum);
            ret1 = id_register_to_tab(&crtabroot, name, NULL, type, ispara, deflinenum);
        }
        if (ret == ERROR || ret1 == ERROR)
            return ERROR;

        /* Add a type to the parameter list of a procedure name */
        if (is_formal_parameter) {
            add_type_to_parameter_list(&globalidroot, current_procedure_name, type);
            add_type_to_parameter_list(&crtabroot, current_procedure_name, type);
        }
    }
    free_strcut_ID(&id_without_type_root);
    free(*type);
    type = NULL;
    return 0;
}

/*!
 * @brief Add a type to the parameter list of a procedure name
 * @param[in] procname procedure name to add parameter to
 * @param[in] type parameter's type
 * @return int Return 0 on success and -1 on failure.
 */
static int add_type_to_parameter_list(struct ID **root, char *procname, struct TYPE **type) {
    struct ID *p_id;
    struct TYPE *p_paratp;
    /* search procedure name */
    /* procedure name is global id */
    if ((p_id = search_tab(root, procname, NULL)) == NULL) {
        fprintf(stderr, "'%s' is not found.", procname);
        return error("procedure name is not found.");
    }
    /* Move to the end of the list */
    p_paratp = p_id->itp;
    while (p_paratp->paratp != NULL) {
        p_paratp = p_paratp->paratp;
    }
    /* add type */
    if ((p_paratp->paratp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
        error("can not malloc1 for struct TYPE in add_type_to_parameter_list\n");
        return ERROR;
    }
    p_paratp->paratp->ttype = (*type)->ttype;
    p_paratp->paratp->arraysize = (*type)->arraysize;

    /* if parameter's type is TPARRAY, parameter's type has element type */
    if ((*type)->ttype & TPARRAY) {
        struct TYPE *p_etype;
        if ((p_etype = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
            return error("can not malloc2 for struct TYPE in add_type_to_parameter_list\n");
        }
        p_etype->ttype = (*type)->ttype;
        p_etype->arraysize = (*type)->arraysize;
        /* element type must be standard type(etp == NULL) */
        p_etype->etp = NULL;
        /* element type doesn't have parameter list */
        p_etype->paratp = NULL;
        /* register element type */
        p_paratp->paratp->etp = p_etype;
    } else {
        /* parameter type is standard type */
        /* standard type doesn't have element type */
        p_paratp->paratp->etp = NULL;
    }
    /* another parameter */
    /* next pointer of parameter list is NULL */
    p_paratp->paratp->paratp = NULL;

    return 0;
}

/*!
 * @brief Create a structure of the standard type
 * @param[in] type Code representing the type
 * @return struct TYPE * Return a pointer to the created structure. 
 */
struct TYPE *std_type(int type) {
    struct TYPE *p_type;
    /* struct TYPE */
    if ((p_type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
        error("can not malloc for struct TYPE in std_type\n");
        return (NULL);
    }
    /* set type only */
    p_type->ttype = type;
    p_type->arraysize = 0;
    p_type->etp = NULL;
    p_type->paratp = NULL;
    return p_type;
}

/*!
 * @brief Create a structure of the array type
 * @param[in] type Code representing the type
 * @return struct TYPE * Return a pointer to the created structure. 
 */
struct TYPE *array_type(int type) {
    struct TYPE *p_type;
    struct TYPE *p_etp;
    /* struct TYPE */
    if ((p_type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
        error("can not malloc1 for struct TYPE in array_type\n");
        return (NULL);
    }
    /* set array type */
    p_type->ttype = type;
    p_type->arraysize = num_attr;

    /* struct TYPE ->etp */
    if ((p_etp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
        error("can not malloc2 for struct TYPE in array_type\n");
        return (NULL);
    }
    /* set element type */
    switch (type) {
        case TPARRAYINT:
            p_etp->ttype = TPINT;
            break;
        case TPARRAYCHAR:
            p_etp->ttype = TPCHAR;
            break;
        case TPARRAYBOOL:
            p_etp->ttype = TPBOOL;
            break;
        default:
            fprintf(stderr, "[%d] is not array type code.\n", type);
            error("type is not array type code.\n");
            return (NULL);
    }
    p_etp->arraysize = 0;
    p_etp->etp = NULL;
    p_etp->paratp = NULL;

    p_type->etp = p_etp;
    p_type->paratp = NULL;
    return p_type;
}

/*!
 * @brief Register the name pointed by name global or local without type
 * @param[in] name Name to be registered
 * @return int Return 0 on success and -1 on failure.
 */
int id_register_without_type(char *name) {
    int ispara = is_formal_parameter;
    int deflinenum = get_linenum();
    if (in_subprogram_declaration) {
        return id_register_to_tab(&id_without_type_root, name, current_procedure_name, NULL, ispara, deflinenum);
    } else {
        return id_register_to_tab(&id_without_type_root, name, NULL, NULL, ispara, deflinenum);
    }
}

/*!
 * @brief Register the name pointed by name root
 * @param[in] root The root of the list struct
 * @param[in] name Name to be registered
 * @param[in] procname procedure name
 * @param[in] type type for a name
 * @param[in] ispara If it is a formal parameter, then 1, otherwise 0
 * @param[in] deflinenum The line number where the name is defined.
 * @return int Return 0 on success and -1 on failure.
 */
static int id_register_to_tab(struct ID **root, char *name, char *procname, struct TYPE **type, int ispara, int deflinenum) {
    struct ID *p_id;
    struct TYPE *p_type;
    char *p_name;
    char *p_procname;

    if ((p_id = search_tab(root, name, procname)) != NULL) {
        fprintf(stderr, "multiple definition of '%s'.\n", name);
        return error("multiple definition");
    }

    /* struct ID */
    if ((p_id = (struct ID *)malloc(sizeof(struct ID))) == NULL) {
        return error("can not malloc1 for struct ID in id_register_to_tab\n");
    }

    /* struct ID ->name */
    if ((p_name = (char *)malloc(strlen(name) + 1)) == NULL) {
        return error("can not malloc2 for name in id_register_to_tab\n");
    }
    strcpy(p_name, name);
    p_id->name = p_name;

    /* struct ID ->procname */
    if (procname == NULL) {
        /* id is global name */
        p_id->procname = NULL;
    } else {
        /* procedure name is registered because id is local name */
        if ((p_procname = (char *)malloc(strlen(procname) + 1)) == NULL) {
            return error("can not malloc3 for procname in id_register_to_tab\n");
        }
        strcpy(p_procname, procname);
        p_id->procname = p_procname;
    }

    /* struct TYPE ->itp */
    if (type == NULL) {
        /* register id without type */
        p_id->itp = NULL;
    } else {
        /* register id with type */
        if ((p_type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
            return error("can not malloc4 for struct TYPE in id_register_to_tab\n");
        }
        p_type->ttype = (*type)->ttype;
        p_type->arraysize = (*type)->arraysize;
        /* if id's type is TPARRAY, id's type has element type */
        if ((*type)->ttype & TPARRAY) {
            struct TYPE *p_etype;
            if ((p_etype = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
                return error("can not malloc5 for struct TYPE in id_register_to_tab\n");
            }
            p_etype->ttype = (*type)->ttype;
            p_etype->arraysize = (*type)->arraysize;
            /* element type must be standard type */
            p_etype->etp = NULL;
            p_etype->paratp = NULL;
            /* register element type */
            p_type->etp = p_etype;
        } else {
            /* id is standard type or procedure */
            p_type->etp = NULL;
        }
        p_type->paratp = NULL;
        p_id->itp = p_type;
    }
    p_id->ispara = ispara;
    p_id->deflinenum = deflinenum;
    p_id->irefp = NULL;

    p_id->nextp = *root;
    *root = p_id;
    return 0;
}

/*!
 * @brief Output the cross reference table
 * @param[in] root pointer cross reference table
 * @return int Return 0 on success and -1 on failure.
 */
int register_linenum(char *name) {
    struct ID *p_id;
    struct ID *p_crtab_id;
    struct LINE *p_line;
    struct LINE *p_crtab_line;
    struct LINE *p_line_tail;
    struct LINE *p_crtab_line_tail;
    if (in_subprogram_declaration) {
        char *procname = current_procedure_name;
        /* search local */
        if ((p_id = search_tab(&localidroot, name, procname)) == NULL) {
            /* search global */
            if ((p_id = search_tab(&globalidroot, name, NULL)) == NULL) {
                fprintf(stderr, "%s was not declared in this scope.", name);
                return error("An undefined name was detected.");
            } else {
                /* id found in global */
                p_crtab_id = search_tab(&crtabroot, name, NULL);
            }
        } else {
            /* id found in local */
            p_crtab_id = search_tab(&crtabroot, name, procname);
        }

        /* recursively called error */
        if (strcmp(name, procname) == 0 && p_id->itp->ttype == TPPROC) {
            fprintf(stderr, "%s is recursively called.", procname);
            return error("procedure is recursively called.");
        }
    } else {
        /* search global */
        if ((p_id = search_tab(&globalidroot, name, NULL)) == NULL) {
            fprintf(stderr, "%s was not declared in this scope.", name);
            return error("An undefined name was detected.");
        } else {
            p_crtab_id = search_tab(&crtabroot, name, NULL);
        }
    }

    if ((p_line = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) {
        return error("can not malloc1 for struct LINE in register_linenum\n");
    }
    p_line->reflinenum = get_linenum();
    p_line->nextlinep = NULL;

    /* Move to the end of the list */
    /* and register linenum */
    p_line_tail = p_id->irefp;
    if (p_line_tail == NULL) {
        p_id->irefp = p_line;
    } else {
        while (p_line_tail->nextlinep != NULL) {
            p_line_tail = p_line_tail->nextlinep;
        }
        p_line_tail->nextlinep = p_line;
    }

    /* for crtab */
    if ((p_crtab_line = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) {
        return error("can not malloc2 for struct LINE in register_linenum\n");
    }
    p_crtab_line->reflinenum = get_linenum();
    p_crtab_line->nextlinep = NULL;

    p_crtab_line_tail = p_crtab_id->irefp;
    if (p_crtab_line_tail == NULL) {
        p_crtab_id->irefp = p_crtab_line;
    } else {
        while (p_crtab_line_tail->nextlinep != NULL) {
            p_crtab_line_tail = p_crtab_line_tail->nextlinep;
        }
        p_crtab_line_tail->nextlinep = p_crtab_line;
    }

    return 0;
}

/*!
 * @brief Output the cross reference table
 * @param[in] root pointer cross reference table
 */
void print_tab(struct ID *root) {
    struct ID *p;
    struct LINE *q;

    fprintf(stdout, "--------------------------------------------------------------------------\n");
    fprintf(stdout, "%-*s", INDENT_SIZE_NAME, "Name");
    fprintf(stdout, "%-*s", INDENT_SIZE_TYPE, "Type");
    fprintf(stdout, "Def. | Ref.\n");
    for (p = root; p != NULL; p = p->nextp) {
        /* Name */
        if (p->procname != NULL) {
            char name_procname[INDENT_SIZE_NAME];
            sprintf(name_procname, "%s:%s", p->name, p->procname);
            fprintf(stdout, "%-*s", INDENT_SIZE_NAME, name_procname);
        } else {
            fprintf(stdout, "%-*s", INDENT_SIZE_NAME, p->name);
        }

        /* Type */
        if (p->itp->ttype == TPPROC) {
            struct TYPE *paratp;
            int remaining_space = INDENT_SIZE_TYPE;

            /* +1 is space of '(' */
            remaining_space -= strlen(typestr[p->itp->ttype]) + 1;
            fprintf(stdout, "%s(", typestr[p->itp->ttype]);

            for (paratp = p->itp->paratp; paratp != NULL; paratp = paratp->paratp) {
                remaining_space -= strlen(typestr[paratp->ttype]);
                fprintf(stdout, "%s", typestr[paratp->ttype]);
                if (paratp->paratp != NULL) {
                    remaining_space -= 1;
                    fprintf(stdout, ",");
                }
            }
            remaining_space -= 1;
            fprintf(stdout, ")");
            fprintf(stdout, "%*s", 0 < remaining_space ? remaining_space : 0, " ");
        } else if (p->itp->ttype & TPARRAY) {
            /* id is array type */
            struct TYPE *p_type = p->itp;
            char str_array_of_type[INDENT_SIZE_TYPE];
            sprintf(str_array_of_type, "array[%d] of %s", p_type->arraysize, typestr[p_type->ttype]);
            fprintf(stdout, "%-*s", INDENT_SIZE_TYPE, str_array_of_type);
        } else {
            /* id is standard type */
            fprintf(stdout, "%-*s", INDENT_SIZE_TYPE, typestr[p->itp->ttype]);
        }

        /* Def. */
        fprintf(stdout, "%*d", INDENT_SIZE_DEF, p->deflinenum);
        /* separator */
        fprintf(stdout, " | ");
        /* Ref. */
        for (q = p->irefp; q != NULL; q = q->nextlinep) {
            fprintf(stdout, "%d", q->reflinenum);
            fprintf(stdout, "%s", q->nextlinep == NULL ? "" : ",");
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "--------------------------------------------------------------------------\n");
    return;
}

/*!
 * @brief Release tha data structure
 */
void release_crtab(void) {
    free_strcut_ID(&globalidroot);
    free_strcut_ID(&localidroot);
    free_strcut_ID(&crtabroot);
    free_strcut_ID(&id_without_type_root);

    init_crtab();
    return;
}

/*!
 * @brief Release tha localidroot and add local id to crtab
 * @return int Return 0 on success and -1 on failure.
 */
int release_localidroot(void) {
    free_strcut_ID(&localidroot);
    return 0;
}

/*!
 * @brief Add id to crtab
 * @param[in] root The root of the list struct
 * @return int Return 0 on success and -1 on failure.
 */
static int add_id_to_crtab(struct ID *root) {
    struct ID *p;
    int ret;

    for (p = root; p != NULL; p = p->nextp) {
        char *name = p->name;
        char *current_procedure_name = p->procname;
        int ispara = p->ispara;
        int deflinenum = p->deflinenum;
        struct TYPE *type = p->itp;
        ret = id_register_to_tab(&crtabroot, name, current_procedure_name, &type, ispara, deflinenum);
        if (ret == ERROR)
            return ERROR;
    }
    return 0;
}

/*!
 * @brief Add global id to crtab
 * @return int Return 0 on success and -1 on failure.
 */
int add_globalid_to_crtab(void) {
    return add_id_to_crtab(globalidroot);
}

/*!
 * @brief Release the struct ID
 * @param[in] root The root of the list struct
 */
static void free_strcut_ID(struct ID **root) {
    struct ID *p, *q;

    for (p = *root; p != NULL; p = q) {
        free(p->name);
        free(p->procname);
        free_struct_TYPE(p->itp);
        free(p->irefp);
        q = p->nextp;
        free(p);
    }
    *root = NULL;
    return;
}

/*!
 * @brief Release the struct TYPE
 * @param[in] root The root of the list struct
 */
static void free_struct_TYPE(struct TYPE *root) {
    struct TYPE *p, *q;

    if (root == NULL) {
        return;
    }

    p = root;

    free(p->etp);
    for (p = root->paratp; p != NULL; p = q) {
        q = p->paratp;
        free(p);
    }
    root = NULL;
    return;
}
