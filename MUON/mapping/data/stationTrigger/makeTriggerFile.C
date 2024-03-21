// make padPos and motif file(s) for trigger.

void makeTriggerFile(char xory, int n)
{
  char padPosFileName[80];
  char motifName[4];
  char motifFileName[80];

  xory = toupper(xory);

  snprintf(padPosFileName,80,"padPos%c%d.dat",xory,n);
  snprintf(motifName,4,"%c%d",xory,n);
  snprintf(motifFileName,80,"motif%s.dat",motifName);

  FILE* fpadpos = fopen(padPosFileName,"w");

  for ( int i = 0; i < n; ++i )
    {
      if ( xory == 'X' )
	{
	  fprintf(fpadpos,"%2d %2d %2d\n",i+1,0,i);
	}
      else
	{
	  fprintf(fpadpos,"%2d %2d %2d\n",i+1,i,0);
	}
    }

  fclose(fpadpos);
  
  FILE* fmotif = fopen(motifFileName,"w");

  fprintf(fmotif,"# Motif %s\n",motifName);

  for ( int i = 0; i < n; ++i )
    {
      fprintf(fmotif,"%2d %2d %2d -\n",i+1,1,i+1);
    }

  fclose(fmotif);
}
