#include "mppl_compiler.h"

struct ID *globalidroot, *localidroot; /*! Pointers to root of global only & local only */

/*! Pointers to root of global + local symbol tables */
struct ID *crtabroot;

/*! Pointers to root of id symbol tables without type */
struct ID *id_without_type_root;

/*! Release the ID struct */
static void free_strcut(struct ID **root);

/*! search the name pointed by name */
static struct ID *search_tab(struct ID **root, char *name, char *procname);

/*! To set the procedure name */
void set_procedure_name(char *name);

/*! Register the name pointed by name root */
static int id_register_to_tab(struct ID **root, char *name, char *procname, struct TYPE **type, int ispara, int deflinenum);

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
            /* when name and p->name are globalid(= name and p->procname are NULL) */
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
    int ret;
    struct ID *p;

    if (type == NULL) {
        return error("struct TYPE is NULL\n");
    }

    for (p = id_without_type_root; p != NULL; p = p->nextp) {
        char *name = p->name;
        char *current_procedure_name = p->procname;
        int ispara = p->ispara;
        int deflinenum = p->deflinenum;
        if (in_subprogram_declaration) {
            ret = id_register_to_tab(&localidroot, name, current_procedure_name, type, ispara, deflinenum);
        } else {
            ret = id_register_to_tab(&globalidroot, name, NULL, type, ispara, deflinenum);
        }
        if (ret == ERROR)
            return ERROR;
    }
    free_strcut(&id_without_type_root);
    free(*type);
    type = NULL;
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
        /*:TODO:*/
        /* print declared linenum */
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
        /* NULL if global name */
        p_id->procname = NULL;
    } else {
        /* Not NULL if local name */
        if ((p_procname = (char *)malloc(strlen(procname) + 1)) == NULL) {
            return error("can not malloc3 for procname in id_register_to_tab\n");
        }
        strcpy(p_procname, procname);
        p_id->procname = p_procname;
    }

    /* struct TYPE ->ttype */
    if (type == NULL) {
        /* register id without type */
        p_id->itp = NULL;
    } else {
        /* struct TYPE */
        if ((p_type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
            return error("can not malloc4 for struct TYPE in id_register_to_tab\n");
        }
        p_type->ttype = (*type)->ttype;
        p_type->arraysize = (*type)->arraysize;
        p_type->etp = (*type)->etp;
        p_type->paratp = (*type)->paratp;
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
 */
void print_tab(struct ID *root) {
    struct ID *p;
    struct LINE *q;

    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Name\tType\tDef. | Ref\n");
    for (p = root; p != NULL; p = p->nextp) {
        fprintf(stdout, "%s", p->name);
        if (p->procname != NULL) {
            fprintf(stdout, ":%s", p->procname);
        }
        fprintf(stdout, "\t");
        fprintf(stdout, "%s\t", typestr[p->itp->ttype]);
        fprintf(stdout, "%d ", p->deflinenum);
        for (q = p->irefp; q != NULL; q = q->nextlinep) {
            fprintf(stdout, "%d", q->reflinenum);
            fprintf(stdout, "%s", q->nextlinep == NULL ? "" : ",");
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "--------------------\n");
    return;
}

/*!
 * @brief Release tha data structure
 */
void release_crtab(void) {
    free_strcut(&globalidroot);
    free_strcut(&localidroot);
    free_strcut(&crtabroot);
    free_strcut(&id_without_type_root);

    init_crtab();
    return;
}

/*!
 * @brief Release tha localidroot and add local id to crtab
 * @return int Return 0 on success and -1 on failure.
 */
int release_localidroot(void) {
    int ret = add_id_to_crtab(localidroot);
    free_strcut(&localidroot);
    return ret;
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
 * @brief Release the data struct
 * @param[in] root The root of the list struct
 */
static void free_strcut(struct ID **root) {
    struct ID *p, *q;

    for (p = *root; p != NULL; p = q) {
        free(p->name);
        free(p->procname);
        free(p->itp);
        free(p->irefp);
        q = p->nextp;
        free(p);
    }
    *root = NULL;
    return;
}
