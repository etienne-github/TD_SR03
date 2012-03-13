#ifndef INIOBJ_H
#define INIOBJ_H

#include <string.h>
#include <stdlib.h>

#include "defobj.h"

#define tablen 3


obj* initTable()
{
	obj* temp = (obj*)malloc(tablen*sizeof(obj));
	
	if (temp) {
	
		strcpy(temp[0].section1,"ident_o1");
		strcpy(temp[0].section2,"description_o1");
		temp[0].ii=11;
		temp[0].jj=12;
		temp[0].dd=10.2345;
		
		strcpy(temp[1].section1,"ident_o2");
		strcpy(temp[1].section2,"description_o2");
		temp[1].ii=21;
		temp[1].jj=22;
		temp[1].dd=20.2345;
		
		strcpy(temp[2].section1,"ident_o3");
		strcpy(temp[2].section2,"description_o3");
		temp[2].ii=31;
		temp[2].jj=32;
		temp[2].dd=30.2345;
		
	}
	
	return temp;
	

}

#endif

