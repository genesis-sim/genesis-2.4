/* chem_struct.h */

#include "struct_defs.h"

/*
*************************************
**         CHEMICAL REACTIONS      **
** Avrama Blackwell, GMU, 1997     **
** modified December, 1999         **
*************************************
*/
struct conserve_type {
    SEGMENT_TYPE
    double previous_state;
    double Conc;
    double quantity;
    double Qinit;
    double Qtot;
    double Cinit;
    double Cmin;
    double Ctot;
    double volume;
    int type;
    double units;   /* 1 for moles, 1e-3 for mmoles, 1e-6 for umoles */
  double Dunits; /* distance units, 1 for meters, 1e-4 for cm */
  double VolUnitConv; /* Dunits^3 - to  convert volume to SI units */
};

struct react_type {
    SEGMENT_TYPE
    double kb;
    double kf;
    double kbprod, kfsubs;
};

struct wgtavg_type {
    SEGMENT_TYPE
    double totValue;
    double totwgt;
    double meanValue;
};

struct enzyme_type {
    SEGMENT_TYPE
    double	conc;
    double      quant;
    double      complex_conc;
    double      complex_quant;
    double	previous_state;
    double	Cmin;
    double	Cinit;
    double      Qinit;
    double	len;   /* Use for 1D or 2D  cylinder, not sphere */
    double      radius; /* use for 1D, 2D cylinder or sphere */
    double	vol;
    double	SAside; /* Use for 1D or 2D cylinder, not sphere */
    double	SAout; /* use for 2D cyl or sphere, not 1D cylinder */
    double	SAin;  /* use for 2D cyl or shell, cross sectional area of illius */
    double units;   /* 1 for moles, 1e-3 for mmoles, 1e-6 for umoles */
    int         type;
    double kf;
    double kb;
    double kcat;
    double deltacat;
    double deltaenz;
    double kbprod;
    double kfsubs;
    double feedback;
    double rateconst;
    double fbconc;
    double thresh;
    int pow;
    int form;
    int sign;
    double halfmax;
  double Dunits; /* distance units, 1 for meters, 1e-4 for cm */
  double VolUnitConv; /* Dunits^3 - to  convert volume to SI units */
};

struct mmenz_type {
    SEGMENT_TYPE
    double Vmax;
    double Km;
    double product;
    float thresh;
    int pos_pow;
    int pos_form;
    float pos_halfmax;
    int neg_pow;
    int neg_form;
    float neg_halfmax;
    float feedback;
    float subs_rate;
};

struct feedback_type {
  SEGMENT_TYPE
    double thresh;
    int pow;
    int form;
    int sign;
    double halfmax;
    double feedback;
};

struct diffusion_type {
    SEGMENT_TYPE
    double D;
    double difflux1, difflux2;
    double units;   /* 1 for moles, 1e-3 for mmoles, 1e-6 for umoles */
  double Dunits; /* distance units, 1 for meters, 1e-4 for cm */
};

struct flux_type {
    SEGMENT_TYPE
    double maxflux;
    double deltaflux1, deltaflux2;
    int power; /* used for exponent for CICR flux */
    double units;   /* 1 for moles, 1e-3 for mmoles, 1e-6 for umoles */
};

struct pump_type {
    SEGMENT_TYPE
    double max_rate;
    double half_conc;
    double half_conc_pow;
    double moles_in, moles_out;
    int power;
    double units;   /* 1 for moles, 1e-3 for mmoles, 1e-6 for umoles */
};

struct exchange_type {
  SEGMENT_TYPE
  double Ca_int;
  double Ca_ext;
  double Na_int;
  double Na_ext;
  double Vm;
  double Vnaca;
  double naterm;
  double caterm;
  double chi;
  double Kmca;
  double Kmna;
  double kmhill;
  double naicao;
  double gamma;
  double T;
  double I;
  double Gbar;
  double Gcurrent;
  double ksat;
  double Vunits;
  int valence;
  int hill;
  int stoich;
  int Na_msg;
  int ncxtype;
};

/* 1-D or 2-D reaction pool object. */
struct rxnpool_type {
    SEGMENT_TYPE
    double	Conc;
    double      quantity;
    double	previous_state;
    double	Cmin;
    double	Cinit;
    double      Qinit;
    double	len;   /* Use for 1D or 2D  cylinder, not sphere */
    double      radius; /* use for 1D, 2D cylinder or sphere */
    double	vol;
    double	SAside; /* Use for 1D or 2D cylinder, not sphere */
    double	SAout; /* use for 2D cyl or sphere, not 1D cylinder */
    double	SAin;
    int         type;
    double units;   /* 1 for moles, 1e-3 for mmoles, 1e-6 for umoles */
    double Iunits;   /* 1e-12 to convert from nA, msec to A sec */
    double Dunits; /* distance units, 1 for meters, 1e-4 for cm */
  double VolUnitConv; /* Dunits^3 - to  convert volume to SI units */
};

struct rhodopsin_type {
  SEGMENT_TYPE
  int input_slice;         /* slice of input isomerization */
  int slice;               /* slice of present element */
  int isom;                /* total_villi *  value of input isomerization */
  float last_isom;         /* floating value of last isomerization */
  int villus;              /* villus of present isomerization */
  int total_villi;         /* number of villi in rhabdomere */
  int slice_villi;         /* number of villi per slice */
  int villi_isom[420];     /* number of isomerizations in each villus */
  double isom_time[420];   /* time of first isomerization in each villus */
  double total_time;       /* time since start of sim */
  int active_villi;        /* number of villi with isomerizations */
  double villus_vol;       /* volume of single villus */
  double villus_xarea;     /* cross sectional area of single villus */
  double villus_sa;        /* surface area of single villus */
  double slice_vol;        /* combined volume of active villi */
  double slice_xarea;      /* combined cross sectional area of active villi */
  double slice_sa;         /* combined surface area of active villi */
  double factor[10];       /*effectiveness of nth isomerization in villus */
  int total_isom;          /* total number of isomerizations (mrho) in slice */
  double depletion;        /* time dependent decrease in mrho effectiveness */
  double effective;        /* combined effectiveness of all mrho in slice */
  int villi_list[420];     /* list of villi with isomerizations */
  double inact_const;      /* Krkcat*Krkf*RKtot/(Krkcat+Krkb) */
  double inact_rate;       /* inact_const * mrho */
  int inact;               /* which villus has an inactivated rhodopsin */
  float last_inact;        /* floating value of last inactivation */
  int total_inact;         /* total number of inactivated mrho */
  double units;            /* 1 for moles, 1e-3 for mmoles, 1e-6 for umoles */
  double	conc;
  double      quantity;
  double	len;   /* of villus */
  double      radius; /* of villus */
};

struct IP3R_type {
	SEGMENT_TYPE
	double fraction;
	double previous_state;
	double alpha;
	double beta;
	double gamma;
	int alpha_state;
	int beta_state;
	int gamma_state;
	int conserve;
	double xinit;
	double xmin;
	double xmax;
};
