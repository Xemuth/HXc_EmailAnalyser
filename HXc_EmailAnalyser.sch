TABLE_(AR)
   STRING_  (MOT, 200)  
   STRING_  (MTYPE,200)
   DOUBLE_  (POUVOIR)
   INT_ (LASTMAJ)
   INT_ (NBAPPARITION)
END_TABLE

TABLE_(RT)
   STRING_  (MOT, 200)  
   STRING_  (MTYPE,200)
   DOUBLE_  (POUVOIR)
   INT_ (LASTMAJ)
   INT_ (NBAPPARITION)
END_TABLE

TABLE_(AN)
   STRING_  (MOT, 200)  
   STRING_  (MTYPE,200)
   DOUBLE_  (POUVOIR)
   INT_ (LASTMAJ)
   INT_ (NBAPPARITION)
END_TABLE


TABLE_(NEURONNES)
	INT_ (ID)
	STRING_ (SPECIALITER_TYPE,200)
	DOUBLE_ (POID)
	INT_ (ACTIVE)
END_TABLE

TABLE_ (LIENS)
	INT_ (NEURONNES1_ID) REFERENCES(NEURONNES)
	INT_ (NEURONNES2_ID) REFERENCES(NEURONNES)
END_TABLE

TABLE_(EMAIL)
	INT_ (EMAILID) PRIMARY_KEY
	STRING_ (SINISTRE,200)
	STRING_ (NIVEAU,200)
	STRING_ (OBJECT,200)
	DATE_ (DATER)
END_TABLE 

TABLE_ (ISLEARNING)
	INT_ (ACTIVE)
END_TABLE
