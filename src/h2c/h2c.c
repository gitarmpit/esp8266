#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
//#include <libgen.h>

static char* parse(char* buf_in, int len, const char* var_name) 
{
    char* buf_out = (char*)malloc (len * 2 * sizeof(char));
    memset (buf_out, 0, len*2*sizeof(char));

    int out_pos = 0;
    char begin[] = "const char %s[] = \"\\\n";
    char end[] = "\";\n";
    sprintf (buf_out, begin, var_name);
    out_pos += strlen(buf_out);

    for (int i = 0; i < len; ++i) 
    {
        char c = buf_in[i];
        if (c == '"') 
        {
           buf_out[out_pos++] = '\\';
        }
        else if (c == '\r') 
        {
           continue;
        }
        else if (c == '\n') 
        {
           buf_out[out_pos++] = '\\';
           buf_out[out_pos++] = 'n';
           buf_out[out_pos++] = '\\';
        }
           
        buf_out[out_pos++] = c;
    }

    strcpy (&buf_out[out_pos], end);

    return buf_out;
}

int main(int argc, char**argv) 
{
  if (argc < 2) 
  {
     printf ("args\n");
     exit(0);
  }

  char* var_name = NULL;

  if (argc == 3) 
  {
    var_name = argv[2];
  }


  char* fname = argv[1];
  char * buffer = 0;
  int length;
  FILE * f = fopen (fname, "rb");

  if (var_name == NULL) 
  {
    var_name = "main_html"; //basename(fname);
    if (var_name == NULL) 
    {
        var_name = fname;
    }

    char* pdot = strrchr (var_name, '.');
    if (pdot) 
    {
        *pdot = '\0';
    }
  }

  if (f)
  {
    fseek (f, 0, SEEK_END);
    length = (int)ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = (char*)malloc (length *sizeof(char));
    if (buffer)
    {
      fread (buffer, 1, length, f);
    }
    fclose (f);
    char* buf_out = parse (buffer, length, var_name);
    printf ("%s\n", buf_out);
    free (buf_out);
  }
  else 
  {
    perror (fname);
  }


  free(buffer);

}