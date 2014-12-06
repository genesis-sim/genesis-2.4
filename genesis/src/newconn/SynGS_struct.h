struct SynG_type
 { CHAN_TYPE
  /* extra fields here */
  double A, B, P, C, R, Gbar, A1, A2, A3, A4, A5, POWER;
};

struct SynE_type
 { CHAN_TYPE
  /* extra fields here */
  double TauPre, TauPost, Vpre, Vpost, Gbar, rectify;
};

struct SynS_type
 {
  CHAN_TYPE
  double m_SynS, A, B, C, D, E;
};

