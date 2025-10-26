#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "ShmMem.h"
#include "MemPool.h"

void main(){
	Block_MemPool::Mem_Manage_Sec FstBlock(8, 2);
	Block_MemPool::Mem_Manage_Spd ScdBlock(8, 2);

	double* Fst = static_cast<double*>(FstBlock.allocate());
	double* Scd = static_cast<double*>(ScdBlock.allocate());

	*Fst = 10;
	*Scd = 10;

	printf("%f\n", *Fst);
	printf("%f\n", *Scd);

	//FstBlock.deallocate(Fst);
	FstBlock.de_allocate();
	//ScdBlock.de_allocate(Scd);
	ScdBlock.de_allocate();

	printf("%d\n", *Fst);
	printf("%d\n", *Scd);

	int debug = 10;

	


}