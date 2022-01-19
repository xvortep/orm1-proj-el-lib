
#include "instruction.h"
#include "header.h"
#include "user.h"
#include <stdio.h>
#include <string.h>

char instr_regex[10][90] = 
{
    /* login regex */
    "^login [a-z]{1,15} [a-z]{1,15}[ \t\n]*$",
    /* logout regex */
    "^logout[ \t\n]*$",
    /* scharll regex */
    "^schall[ \t\n]*$",                            
    /* chkst regex */   
    "^chkst[ \t\n]*$",           
    /* download regex */                  
    "^downl [0-9]{1,9}[ \t\n]*$",
    /* search regex */
    "^search i:[0-9]{1,9} a:[a-z]{1,16} [a-z]{1,16} t:[a-z]{1,16} y:[0-9]{1,9}[ \t\n]*$",
    /* search by id regex */
    "^search i:[0-9]{1,9}[ \t\n]*$",
    /* search by author regex */
    "^search a:[a-z]{1,16} [a-z]{1,16}[ \t\n]*$",
    /* search by trunc_name regex */
    "^search t:[a-z]{1,16}[ \t\n]*$",
    /* search by year regex */
    "^search y:[0-9]{1,9}[ \t\n]*$",
};

void print_instr(const Instruction *i)
{
    char str[200];
    char tmp[200];
    switch (i->instr) {    
        case LOGIN:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> login\t\t\tuser: %s" ANSI_COLOR_RESET, utos(&i->inf.usr, tmp));
            break;
        case LOGOUT:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> logout" ANSI_COLOR_RESET);
            break;
        case SCHALL:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> schall" ANSI_COLOR_RESET);
            break;
        case CHKST:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> chkst" ANSI_COLOR_RESET);
            break;
        case DOWNL:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> downl\t\t\tcriteria: %d" ANSI_COLOR_RESET, i->inf.hdr.id);
            break;
        case SEARCH:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> search\t\t\tcriteria: %s" ANSI_COLOR_RESET, htos(i->inf.hdr, tmp));
            break;
        case SEARCH_I:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> search by id\t\tcriteria: %s" ANSI_COLOR_RESET, htos(i->inf.hdr, tmp));
            break;
        case SEARCH_A:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> search by author\t\tcriteria: %s" ANSI_COLOR_RESET, htos(i->inf.hdr, tmp));
            break;
        case SEARCH_T:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> search by trunc_name\tcriteria: %s" ANSI_COLOR_RESET, htos(i->inf.hdr, tmp));
            break;
        case SEARCH_Y:
            sprintf(str, ANSI_COLOR_GREEN "instruction -> search by year\t\tcriteria: %s" ANSI_COLOR_RESET, htos(i->inf.hdr, tmp));
            break;
        default:
            sprintf(str, ANSI_COLOR_RED "instruction -> invalid instruction" ANSI_COLOR_RESET);
            break;
    }
    printf("%s\n", str);
}

void fetch_args(const char *str, Instruction *i)
{
    char a[16];
    int fetchedargs;
    switch (i->instr) {
        case LOGIN:
            sscanf(str, "login %s %s", i->inf.usr.id, i->inf.usr.pass);
            break;
        case DOWNL:
            sscanf(str, "downl %d", &i->inf.hdr.id);
            break;
        case SEARCH:
            fetchedargs = sscanf(str, "search i:%d a:%s %s t:%s y:%d", 
            &i->inf.hdr.id, 
            i->inf.hdr.author,
            a,
            i->inf.hdr.trunc_name,
            &i->inf.hdr.year
            );
            if (fetchedargs != 5) {
                /* error in reading */
                i->instr = NO_INSTR;
            } else {
                strcat(i->inf.hdr.author, " ");
                strcat(i->inf.hdr.author, a);
            }
            break;
        case SEARCH_I:
            sscanf(str, "search i:%d", &i->inf.hdr.id);
            break;
        case SEARCH_A:
            sscanf(str, "search a:%s %s", i->inf.hdr.author, a);
            strcat(i->inf.hdr.author, " ");
            strcat(i->inf.hdr.author, a);
            break;
        case SEARCH_T:
            sscanf(str, "search t:%s", i->inf.hdr.trunc_name);
            break;
        case SEARCH_Y:
            sscanf(str, "search y:%d", &i->inf.hdr.year);
            break;
        default:
            break;
    }
}

instr_t parse_instr(const char *str, Instruction *i)
{
    regex_t regex;
    int reti = 1;
    char msgbuf[200];
    instr_t which = LOGIN;

    while (which < NO_INSTR) {
        reti = regcomp(&regex, instr_regex[which], REG_EXTENDED);
        if (reti != 0) {
            puts("could not compile regex.");
            which = NO_INSTR;
            regfree(&regex);
            break;
        } else {
            reti = regexec(&regex, str, 0, NULL, 0);
            if (reti == 0) {
                //puts("match");
                i->instr = which;
                regfree(&regex);
                break;
            } else {
                //puts("nomatch");
                ++which;
                regfree(&regex);
            }
        }
    }
    i->instr = which;
    fetch_args(str, i);
    return which;
}
