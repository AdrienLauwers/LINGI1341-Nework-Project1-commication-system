#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <float.h>

int main (int argc, char ** argv){
  int c;
  double result = 0;

  while (1){
    double operand;

    static struct option long_options[] =
    {
      {"ADD", required_argument, 0, 'a'},
      {"SUB", required_argument, 0, 's'},
      {"MUL", required_argument, 0, 'm'},
      {"DIV", required_argument, 0, 'd'},
      {"INC", no_argument, 0, 'I'},
      {"DEC", no_argument, 0, 'D'},
      {0,0,0,0}
    };
    int option_index = 0;

    c = getopt_long (argc, argv, "a:d:s:m:ID", long_options, &option_index);
    char * end;
    if (c == -1)
    break;
    switch (c){

      case 'a':
      operand = strtod(optarg, &end);
      if(*end != '\0'){
        fprintf(stderr, "Argument n'est pas une valeur numérique %s\n", optarg);
        return 1;
      }
      result+=operand;;
      break;

      case 's':
      operand = strtod(optarg, &end);
      if(*end != '\0'){
        fprintf(stderr, "Argument n'est pas une valeur numérique %s\n", optarg);
        return 1;
      }
      result-=operand;
      break;

      case'm':
      operand = strtod(optarg, &end);
      if(*end != '\0'){
        fprintf(stderr, "Argument n'est pas une valeur numérique %s\n", optarg);
        return 1;
      }
      result*=operand;
      break;

      case'd':
      operand = strtod(optarg, &end);
      if(*end != '\0'){
        fprintf(stderr, "Argument n'est pas une valeur numérique %s\n", optarg);
        return 1;
      }
      result/=operand;
      break;

      case 'I':
      result+=1;
      break;

      case 'D':
      result-=1;
      break;

      case '?':
      if(optopt == 'c')
      fprintf(stderr, "Option -%c requiert un argument.\n", optopt);

      else
      fprintf(stderr, "Character d'option inconnu '\\x%x'. \n", optopt);
      return 1;
      default:
      abort();
    }
  }
  if(optind < argc){
    char * end;
    int precision = (int) strtod(argv[optind++], &end);
    if(*end != '\0')
    return -1;
    printf("%.*f\n",precision ,result);
  }
  else{
    int result_int= (int) result;
    printf("%d\n", result_int);
  }
  return EXIT_SUCCESS;
}
