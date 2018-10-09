#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef enum { false, true } bool;

typedef struct UserEquipment {
	int id;
	int pdcchFormat;
	int metric;
	int pdb;
	int delay;
} UE;

int PDCCHNumberOfCCEs[4] = {
  1, 2, 4, 8
};

int PDCCHNumberOfCandidates_UESpecific[4] = {
  6, 6, 2, 2
};

int PDCCHNumberOfCandidates_Common[4] = {
  -1, -1, 4, 2
};

int nSubframe, totalUEsAllocated = 0;

UE *setUserEquipments(int nUes);

void aggregationLevel(int nUes, UE *userEquipments, double pbAl1, double pbAl2, double pbAl4, double pbAl8);

int *buildPdcchVector(int nCces);

bool PDCCHResourcesAllocation(int RNTI, int *PDCCHResources, UE *userEquipments, int nCCEs);

bool PDCCHResourcesAllocation1(int m, int i, int RNTI, int *PDCCHResources, UE *userEquipments, int nCCEs);

int Calc_CCE_Index(int m, int i, int RNTI, UE *userEquipments, int nCCEs);

int Calc_Y(int k, int RNTI);

int cmpfunc (const void *a, const void *b) {
    UE *ia = (UE *)a;
    UE *ib = (UE *)b;
    
    return (int)(ia->metric - ib->metric);
}

void setMetric(int nUEs, UE *userEquipments) {
	int i;
	for (i = 0 ; i < nUEs ; i++) {
		userEquipments[i].metric = (rand() % 5000);
	}
}

void BestEffortCCEsScheduling(int nUEs, UE *userEquipments, int nCCEs, int *pdcchResources) {
	int i;
	int currentCces = nCCEs;
	for (i = 0 ; i < nUEs ; i++) {
		if (currentCces <= 0) {
			return;
		}
		if (PDCCHResourcesAllocation(i, pdcchResources, userEquipments, nCCEs) == true) {
			currentCces -= PDCCHNumberOfCCEs[userEquipments[i].pdcchFormat];
			totalUEsAllocated++;
		}
	}
}

void NewCCEsScheduling(int nUEs, UE *userEquipments, int nCCEs, int *pdcchResources) {
	int i, pdcchFormat, m, j, maxCCEs, cceIndex;
	int PDCCHCcesPerUser[nUEs][nCCEs];
	int PDCCHPositionPerAggregation[nUEs][nCCEs];
	int currentCces = nCCEs;
	
	for (i = 0 ;  i < nUEs ; i++) {
		for (j = 0 ; j < nCCEs ; j++) {
			PDCCHCcesPerUser[i][j] = 0;
			PDCCHPositionPerAggregation[i][j] = 0;
		}
	}
	
	for (i = 0 ; i < nUEs ; i++) {

		pdcchFormat = userEquipments[i].pdcchFormat;

    maxCCEs = PDCCHNumberOfCCEs[pdcchFormat];
    
    //printf("%d -", userEquipments[i].id);

    for (m = 0 ; m < PDCCHNumberOfCandidates_UESpecific[pdcchFormat] ; m++) {
    	//printf(" [ ");
      for (j = 0 ; j < maxCCEs ; j++) {
        cceIndex = Calc_CCE_Index(m, j, i, userEquipments, nCCEs);
        PDCCHCcesPerUser[i][cceIndex] = PDCCHNumberOfCCEs[pdcchFormat];
        PDCCHPositionPerAggregation[i][cceIndex] = j + 1;
        //printf("%d ", cceIndex);
      }
      //printf("]");
    }
    //printf("\n");
  }
  
  for (i = 0 ;  i < nUEs ; i++) {
  	//printf("[%d, %d, %d]\t[ ", userEquipments[i].id, PDCCHNumberOfCCEs[userEquipments[i].pdcchFormat], PDCCHNumberOfCandidates_UESpecific[userEquipments[i].pdcchFormat]);
		for (j = 0 ; j < nCCEs ; j++) {
			//printf("%d ",PDCCHCcesPerUser[i][j]);
		}
		//printf("]\n");
	}
	
	//printf("\n\n");
	
	for (i = 0 ;  i < nUEs ; i++) {
  	//printf("[%d, %d, %d]\t[ ", userEquipments[i].id, PDCCHNumberOfCCEs[userEquipments[i].pdcchFormat], PDCCHNumberOfCandidates_UESpecific[userEquipments[i].pdcchFormat]);
		for (j = 0 ; j < nCCEs ; j++) {
			//printf("%d ",PDCCHPositionPerAggregation[i][j]);
		}
		//printf("]\n");
	}
	
	int h, where, total, k, p;
	for (i = 0 ; i < nUEs && currentCces > 0; i++) {

		pdcchFormat = userEquipments[i].pdcchFormat;

		h = 1;
		where = -1;
		total = 100000;
		
		for (j = 0; j < nCCEs; j += h) {
      h = 1;
      if (PDCCHCcesPerUser[i][j] != 0) {
        int soma = 0;
        int first = j;

        for (k = 0; k < PDCCHNumberOfCCEs[pdcchFormat]; k++) {
          if (pdcchResources[j + k] == -2) {
            for (p = i + 1; p < nUEs; p++) {
              //soma += PDCCHCcesPerUser[p][j + k];
              if (PDCCHCcesPerUser[p][j + k] > 0) {
                soma += 1;
              }
            }

            h = PDCCHNumberOfCCEs[pdcchFormat];
          } else {
            soma = 1000000;
            h = PDCCHNumberOfCCEs[pdcchFormat];
            break;
          }
        }

        if (soma <= total) {
          where = first;
          total = soma;
        }
      }
    }
    
    //where = -1;
    if (where > -1) {
    	//m_UEsScheduled_temp++;
      for (k = 0 ; k < PDCCHNumberOfCCEs[pdcchFormat] ; k++) {
        pdcchResources[where + k] = userEquipments[i].id;
      }
      currentCces -= PDCCHNumberOfCCEs[userEquipments[i].pdcchFormat];
      totalUEsAllocated++;
    } else {
//#ifdef PDCCH_DEBUG
      //std::cout << "UE Blocked in PDCCH" << std::endl;
//#endif
    }

  }
  
}

int main(int argc, const char* argv[]) {
	srand((unsigned) atoi(argv[1]));
	
	int nUEs = atoi(argv[2]);
	int nCCEs = atoi(argv[3]);
	int nSubframes = atoi(argv[4]);
	
	int ccesNotUsed = 0, p = 0;

	UE *userEquipments = setUserEquipments(nUEs);
	
	aggregationLevel(nUEs, userEquipments, 0.40, 0.25, 0.20, 0.15);
	
	for (nSubframe = 0 ; nSubframe < nSubframes ; nSubframe++) {
		printf("Subframe %d:\n", nSubframe);
		
		setMetric(nUEs, userEquipments);
	
		qsort(userEquipments, nUEs, sizeof(UE), cmpfunc);
		
		int i;
		for (i = 0 ; i < nUEs ; i++) {
			printf("%d %d %d %d\n", i, userEquipments[i].id, (int) pow(2, userEquipments[i].pdcchFormat), userEquipments[i].metric);
		}
	
		int *pdcchResources = buildPdcchVector(nCCEs);
	
		for (i = 0 ; i < nCCEs ; i++) {
			//printf("%d %d\n", i, pdcchResources[i]);
		}
	
		totalUEsAllocated = 0;

		BestEffortCCEsScheduling(nUEs, userEquipments, nCCEs, pdcchResources);

		//NewCCEsScheduling(nUEs, userEquipments, nCCEs, pdcchResources);

		int j, n = 0;
		for (i = 0 ; i < nUEs && n < totalUEsAllocated ; i++) {
			for (j = 0 ; j < nCCEs ; j++) {
				if (pdcchResources[j] == userEquipments[i].id) {
					n++;
					break;
				}
			}
			if (j == nCCEs)
				p++;
		}
	
		for (i = 0 ; i < nCCEs ; i++) {
			if (pdcchResources[i] == -2) {
				ccesNotUsed++;
			}
			printf("%d %d\n", i, pdcchResources[i]);
		}
	}
	
	printf("\n\nTotal = %d; CCEs Not Used = %d %f; x = %d\n", totalUEsAllocated, ccesNotUsed, (ccesNotUsed / (double) (nSubframes * nCCEs)) * 100.0, p);
	
	return 0;
}

UE *setUserEquipments(int nUes) {
	UE *userEquipments = malloc(nUes * sizeof(UE));
	
	int i;
	for (i = 0 ; i < nUes ; i++) {
		
		userEquipments[i].id = (i + 1);
	}
	
	return userEquipments;
}

void aggregationLevel(int nUes, UE *userEquipments, double pbAl1, double pbAl2, double pbAl4, double pbAl8) {
	int nUesAl1 = floor(nUes * pbAl1);
	int nUesAl2 = floor(nUes * pbAl2);
	int nUesAl4 = floor(nUes * pbAl4);
	int nUesAl8 = nUes - (nUesAl1 + nUesAl2 + nUesAl4);
	
	int i;
	for (i = 0 ; i < nUes ; i++) {
		if (i < nUesAl1) {
			userEquipments[i].pdcchFormat = 0;
		} else if (i < nUesAl1 + nUesAl2) {
			userEquipments[i].pdcchFormat = 1;
		} else if (i < nUesAl1 + nUesAl2 + nUesAl4) {
			userEquipments[i].pdcchFormat = 2;
		} else if (i < nUesAl1 + nUesAl2 + nUesAl4 + nUesAl8){
			userEquipments[i].pdcchFormat = 3;
		}
	}
}

int *buildPdcchVector(int nCces) {
	int *pdcch = malloc(nCces * sizeof(int));
	
	int i;
	for (i = 0 ; i < nCces ; i++) {
		pdcch[i] = -2;
	}
	
	return pdcch;
}

bool PDCCHResourcesAllocation(int RNTI, int *PDCCHResources, UE *userEquipments, int nCCEs) {
  int pdcchFormat, m;
//#ifdef PDCCH_DEBUG
 //printf("PDCCH Allocation Start of the UE %d!\n", RNTI);
//#endif

  if (RNTI == -1) {
  	
    for (m = 0 ; m < PDCCHNumberOfCandidates_Common[3] ; m++) {
      if (PDCCHResourcesAllocation1(m, 0, RNTI, PDCCHResources, userEquipments, nCCEs) == true) {
//#ifdef PDCCH_DEBUG
        //printf("PDCCH Allocation Stop of the UE %d = YES!\n", RNTI);
//#endif
        return true;
      }
    }
  } else {
  pdcchFormat = userEquipments[RNTI].pdcchFormat;

    for (m = 0 ; m < PDCCHNumberOfCandidates_UESpecific[pdcchFormat] ; m++) {
//#ifdef PDCCH_DEBUG
      //printf("PDCCH %d attempt to allocate of the %d!\n", m + 1, RNTI);
//#endif
      if (PDCCHResourcesAllocation1(m, 0, RNTI, PDCCHResources, userEquipments, nCCEs) == true) {
//#ifdef PDCCH_DEBUG
				//printf("PDCCH Allocation Stop of the UE %d = YES!\n", RNTI);
//#endif
        return true;
      }
    }
  }

//#ifdef PDCCH_DEBUG
  //printf("PDCCH Allocation Stop of the UE %d = NO!\n", RNTI);
//#endif
  return false;
}

bool PDCCHResourcesAllocation1(int m, int i, int RNTI, int *PDCCHResources, UE *userEquipments, int nCCEs) {
  int cceIndex, maxCCEs;

  if (RNTI == -1) {
    maxCCEs = 8;
  } else {
  	maxCCEs = PDCCHNumberOfCCEs[userEquipments[RNTI].pdcchFormat];
  }
//#ifdef PDCCH_DEBUG
  //printf("PDCCH CCEs for UE %d is %d!\n", RNTI, maxCCEs);
//#endif

  if (i == maxCCEs) {
    return true;
  }

  cceIndex = Calc_CCE_Index(m, i, RNTI, userEquipments, nCCEs);

  if (cceIndex == -1) {
    return false;
  }

//#ifdef PDCCH_DEBUG
  //printf("CCE_Index of the UE %d on TTI %d is %d!\n", RNTI, RNTI, cceIndex);
//#endif

  if (PDCCHResources[cceIndex] != -2) {
//#ifdef PDCCH_DEBUG
    //printf("PDCCH Allocation of the UE %d is false!\n", RNTI);
//#endif
    return false;
  } else {
    if (PDCCHResourcesAllocation1(m, i + 1, RNTI, PDCCHResources, userEquipments, nCCEs) == true) {
      PDCCHResources[cceIndex] = userEquipments[RNTI].id;
      return true;
    } else {
      PDCCHResources[cceIndex] = -2;
      return false;
    }
  }
}

int Calc_CCE_Index(int m, int i, int RNTI, UE *userEquipments, int nCCEs) {
	int C = -1;

	int L = PDCCHNumberOfCCEs[userEquipments[RNTI].pdcchFormat];
	int k = nSubframe % 10; //FrameManager::Init()->GetNbSubframes();
	int N = nCCEs; //m_mac->GetDevice()->GetPhy()->GetBandwidthManager()->GetPDCCHTotalCces();
	int T = floor(N / L);

	C = L * ((Calc_Y(k, userEquipments[RNTI].id) + m) % T) + i;

	return C;
}

int Calc_Y(int k, int RNTI) {
  long int y, t;

  /* Common search space */
  if (RNTI == -1)
    return 0;

  /* Dedicate search space */
  if (k < 0) {
    y = RNTI;
  } else {
    t = Calc_Y(k - 1, RNTI);
    y = (39827 * t) % 65537;
  }

  return y;
}