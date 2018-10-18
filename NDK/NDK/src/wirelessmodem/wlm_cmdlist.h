/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2012-08-17
* 版	本：	V1.00
* 最后修改人：csl
* 最后修改日期：
*/
#ifndef WLM_CMDLIST_H_INCLUDED
#define WLM_CMDLIST_H_INCLUDED

#include "wirelessmodem.h"

/**
*应用程序传入的公用命令列表
*/
const WLMATCMDMAP Public_cmd_List[]= {
    {WLM_CMD_E0,	"E0"		},
    {WLM_CMD_CSQ,	"+CSQ"		},
    {WLM_CMD_CREG,	"+CREG?"	},
    {WLM_CMD_CREG,	"+CAD?"		},
    {WLM_CMD_CPIN,	"+CPIN?"	},
    {WLM_CMD_CPIN0,	"+CPIN="	},
    {WLM_CMD_CGATT0,"+CGATT=0"	},
    {WLM_CMD_CGATT1,"+CGATT=1"	},
    {WLM_CMD_DIAL,	"D*99***1#"	},
    {WLM_CMD_DIAL,	"D#777"		},
    {WLM_CMD_D2,	"&D2"},
    {WLM_CMD_COPS,	"+COPS?"	},
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_UNDEFINE, NULL		}
};

const WLMRESPTABLE Public_ret_List[]= {
    {WLM_STATUS_RET_ERROR,	"ERROR",		NULL	},
    {WLM_STATUS_OK,				"OK",			NULL	},
    {WLM_STATUS_CONNECT,		"CONNECT",		NULL	},
    {WLM_STATUS_RING,		"RING",			NULL	},
    {WLM_STATUS_NO_CARRIER,	"NO CARRIER",	NULL	},
    {WLM_STATUS_NO_CARRIER,	"NO DIALTONE",	NULL	},
    {WLM_STATUS_NO_CARRIER,	"FAIL",			"ERROR"	},
    {WLM_STATUS_NO_CARRIER,  "BUSY",         NULL},
    {WLM_STATUS_UNTYPED,		NULL,			NULL	}
};

///<SM300命令映射表
const WLMATCMDMAP SM300_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ"		},
    {WLM_CMD_CREG,		"+CREG?"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN="	},
    {WLM_CMD_CGATT0,	"+CGATT=0"	},
    {WLM_CMD_CGATT1,	"+CGATT=1"	},
    {WLM_CMD_DIAL,		"D*99***1#"	},
    {WLM_CMD_D2,		"&D2"},
    {WLM_CMD_COPS,		"+COPS?"	},
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMCMDRESPTABLE SM300_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};

/*命令映射表*/
const WLMATCMDMAP MC39i_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ"		},
    {WLM_CMD_CREG,		"+CREG?"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN="	},
    {WLM_CMD_CGATT0,	"+CGATT=0"	},
    {WLM_CMD_CGATT1,	"+CGATT=1"	},
    {WLM_CMD_DIAL,		"D*99***1#"	},
    {WLM_CMD_D2,		"&D2"},
    {WLM_CMD_COPS,		"+COPS?"	},
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMCMDRESPTABLE MC39I_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP DTGS800X_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ?"		},
    {WLM_CMD_CREG,		"+CAD?"		},
    {WLM_CMD_CPIN,		"+RLOCK?"	},
    {WLM_CMD_CPIN0,		"+RLOCK=1,"	},
    {WLM_CMD_CGATT0,	""			},
    {WLM_CMD_CGATT1,	""			},
    {WLM_CMD_DIAL,		"D#777"		},
    {WLM_CMD_D2,		"&D2"       },
    {WLM_CMD_COPS,		""	},
    {WLM_CMD_CGMR,      "+GMR"      },
    {WLM_CMD_CGSN,      "+ESN"      },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMRESPTABLE DTGS800X_CPIN_LIST[]= {
    {WLM_STATUS_UNTYPED,	    "NORUIM",	    "ERROR"	},
    {WLM_STATUS_UNTYPED,	    "NOLOCK",	    "READY" },
    {WLM_STATUS_UNTYPED,	    "READY",	    NULL	},
    {WLM_STATUS_UNTYPED,	    "PIN",	        "SIM PIN"},
    {WLM_STATUS_OK,		        "PASS",		    "OK"	},
    {WLM_STATUS_UNTYPED,	    "FAIL",		    "ERROR"	},
    {WLM_STATUS_OK,		        "+CPIN:",	    NULL	},
    {WLM_STATUS_UNTYPED,	    NULL,		    NULL	}
};

const WLMCMDRESPTABLE DTGS800X_RET_List[]= {
    {WLM_CMD_CPIN, (WLMRESPTABLE *)&DTGS800X_CPIN_LIST},
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP DTM228C_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ?"		},
    {WLM_CMD_CREG,		"+CAD?"		},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN=1,"	},
    {WLM_CMD_CGATT0,	""			},
    {WLM_CMD_CGATT1,	""			},
    {WLM_CMD_DIAL,		"D#777"		},
    {WLM_CMD_D2,		"&D2"       },
    {WLM_CMD_COPS,		""	},
    {WLM_CMD_CGMR,      "+GMR"      },
    {WLM_CMD_CGSN,      "+ESN"      },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMCMDRESPTABLE DTM228C_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP MC8331_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ?"		},
    {WLM_CMD_CREG,		"+CAD?"		},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN=1,"	},
    {WLM_CMD_CGATT0,	""			},
    {WLM_CMD_CGATT1,	""			},
    {WLM_CMD_DIAL,		"D#777"		},
    {WLM_CMD_D2,		"&D2"       },
    {WLM_CMD_COPS,		""	},
    {WLM_CMD_CGMR,      "+GMR"      },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};
const WLMRESPTABLE MC8331_CAD_List[]= {
    {WLM_STATUS_UNTYPED,     "1", "+CAD: 1"},
    {WLM_STATUS_UNTYPED,     "0", "+CAD: 1"},
    {WLM_STATUS_UNTYPED,	    NULL,		 NULL	}
};

const WLMCMDRESPTABLE MC8331_RET_List[]= {
    {WLM_CMD_CREG, (WLMRESPTABLE *)&MC8331_CAD_List},
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP EM200_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ?"		},
    {WLM_CMD_CREG,		"%STATE"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN=1,"	},
    {WLM_CMD_CGATT0,	""			},
    {WLM_CMD_CGATT1,	""			},
    {WLM_CMD_DIAL,		"D#777"		},
    {WLM_CMD_D2,		"&D2"       },
    {WLM_CMD_COPS,		""			},
    {WLM_CMD_CGMR,      "+CVER"     },
    {WLM_CMD_CGSN,      "+ESN"      },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMRESPTABLE EM200_CAD_List[]= {
    {WLM_STATUS_UNTYPED,     "%STATE:2", "+CAD: 1"},
    {WLM_STATUS_UNTYPED,	    NULL,		 NULL	}
};

const WLMCMDRESPTABLE EM200_RET_List[]= {
    {WLM_CMD_CREG, (WLMRESPTABLE *)&EM200_CAD_List},
    {WLM_CMD_UNDEFINE, NULL}
};


const WLMATCMDMAP M72_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ"		},
    {WLM_CMD_CREG,		"+CREG?"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN="	},
    {WLM_CMD_CGATT0,	"+CGATT=0"	},
    {WLM_CMD_CGATT1,	"+CGATT=1"	},
    {WLM_CMD_DIAL,		"D*99***1#"	},
    {WLM_CMD_D2,		"&D2"},
    {WLM_CMD_COPS,		"+COPS?"	},
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_CCID,      "+QCCID"     },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMCMDRESPTABLE M72_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};
const WLMATCMDMAP BGS2_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ"		},
    {WLM_CMD_CREG,		"+CREG?"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN="	},
    {WLM_CMD_CGATT0,	"+CGATT=0"	},
    {WLM_CMD_CGATT1,	"+CGATT=1"	},
    {WLM_CMD_DIAL,		"D*99***1#"	},
    {WLM_CMD_D2,		"&D2"},
    {WLM_CMD_COPS,		"+COPS?"	},
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_CCID,      "+CRSM=176,12258,0,0,10"     },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMCMDRESPTABLE BGS2_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP EHS5_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ"		},
    {WLM_CMD_CREG,		"+CREG?"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN="	},
    {WLM_CMD_CGATT0,	"+CGATT=0"	},
    {WLM_CMD_CGATT1,	"+CGATT=1"	},
    {WLM_CMD_DIAL,		"D*99***1#"	},
    {WLM_CMD_D2,		"&D2"},
    {WLM_CMD_COPS,		"+COPS?"	},
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_CCID,      "+CCID"     },
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};

const WLMCMDRESPTABLE EHS5_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};
///G610命令映射表
const WLMATCMDMAP G610_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ"		},
    {WLM_CMD_CREG,		"+CREG?"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN="	},
    {WLM_CMD_CGATT0,	"+CGATT=0"	},
    {WLM_CMD_CGATT1,	"+CGATT=1"	},
    {WLM_CMD_DIAL,		"D*99***1#"	},
    {WLM_CMD_D2,		"&D2"},
    {WLM_CMD_COPS,		"+COPS?"	},
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_CCID,     "+CCID"},
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};
//const WLMRESPTABLE G610_GTGIS_List[]= {
////    {NET_OK,     "+GTGIS:", NULL},
//    {NET_MODEM_UNTYPED,	    NULL,		 NULL	}
//};
const WLMCMDRESPTABLE G610_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP CE910_CMD_List[]= {
    {WLM_CMD_E0,		"E0"		},
    {WLM_CMD_CSQ,		"+CSQ"		},
    {WLM_CMD_CREG,		"+CREG?"	},
    {WLM_CMD_CPIN,		"+CPIN?"	},
    {WLM_CMD_CPIN0,		"+CPIN=1,"	},
    {WLM_CMD_CGATT0,	""			},
    {WLM_CMD_CGATT1,	""			},
    {WLM_CMD_DIAL,		"D#777"		},
    {WLM_CMD_D2,		"&D2"       },
    {WLM_CMD_COPS,		""	},
    {WLM_CMD_CGMR,      "+GMR"      },
    {WLM_CMD_CGSN,      "#CGSN"      },
    {WLM_CMD_CCID,			"+CCID"},
    {WLM_CMD_END,       ""	        },
    {WLM_CMD_UNDEFINE,	NULL		}
};
const WLMRESPTABLE CE910_CREG_List[] ={
		{WLM_STATUS_UNTYPED,",1",": 1"},//为了兼容应用层公共库不做修改。
		{WLM_STATUS_UNTYPED,NULL,NULL}
};

const WLMCMDRESPTABLE CE910_RET_List[]= {
		{WLM_CMD_CREG,(WLMRESPTABLE *)&CE910_CREG_List},
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP DE910_CMD_List[]= {
    {WLM_CMD_E0,        "E0"        },
    {WLM_CMD_CSQ,       "+CSQ"      },
    {WLM_CMD_CREG,      "+CREG?"    },
    {WLM_CMD_CPIN,      "+CPIN?"    },
    {WLM_CMD_CPIN0,     "+CPIN=1,"  },
    {WLM_CMD_CGATT0,    ""          },
    {WLM_CMD_CGATT1,    ""          },
    {WLM_CMD_DIAL,      "D#777"     },
    {WLM_CMD_D2,        "&D2"       },
    {WLM_CMD_COPS,      ""  },
    {WLM_CMD_CGMR,      "+GMR"      },
    {WLM_CMD_CGSN,      "#CGSN"      },
    {WLM_CMD_CCID,          "+CCID"},
    {WLM_CMD_END,       ""          },
    {WLM_CMD_UNDEFINE,  NULL        }
};
const WLMRESPTABLE DE910_CREG_List[] = {
    {WLM_STATUS_UNTYPED,",1",": 1"},//为了兼容应用层公共库不做修改。
    {WLM_STATUS_UNTYPED,NULL,NULL}
};

const WLMCMDRESPTABLE DE910_RET_List[]= {
    {WLM_CMD_CREG,(WLMRESPTABLE *)&DE910_CREG_List},
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP H350_CMD_List[]= {
    {WLM_CMD_E0,        "E0"        },
    {WLM_CMD_CSQ,       "+CSQ"      },
    {WLM_CMD_CREG,      "+CREG?"    },
    {WLM_CMD_CPIN,      "+CPIN?"    },
    {WLM_CMD_CPIN0,     "+CPIN="    },
    {WLM_CMD_CGATT0,    "+CGATT=0"  },
    {WLM_CMD_CGATT1,    "+CGATT=1"  },
    {WLM_CMD_DIAL,      "D*99***1#" },
    {WLM_CMD_D2,        "&D2"},
    {WLM_CMD_COPS,      "+COPS?"    },
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+GSN"      },
    {WLM_CMD_CCID,     "+CCID"},
    {WLM_CMD_END,       ""          },
    {WLM_CMD_UNDEFINE,  NULL        }
};

const WLMCMDRESPTABLE H350_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP QW200_CMD_List[]= {
    {WLM_CMD_E0,        "E0"        },
    {WLM_CMD_CSQ,       "+CSQ"      },
    {WLM_CMD_CREG,      "+CREG?"    },
    {WLM_CMD_CPIN,      "+CPIN?"    },
    {WLM_CMD_CPIN0,     "+CPIN="    },
    {WLM_CMD_CGATT0,    "+CGATT=0"  },
    {WLM_CMD_CGATT1,    "+CGATT=1"  },
    {WLM_CMD_DIAL,      "D*99***1#" },
    {WLM_CMD_D2,        "&D2"},
    {WLM_CMD_COPS,      "+COPS?"    },
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+CGSN"      },
    {WLM_CMD_CCID,      "+CCID"     },
    {WLM_CMD_END,       ""          },
    {WLM_CMD_UNDEFINE,  NULL        }
};

const WLMCMDRESPTABLE QW200_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};

const WLMATCMDMAP SIM800C_CMD_List[]= {
    {WLM_CMD_E0,        "E0"        },
    {WLM_CMD_CSQ,       "+CSQ"      },
    {WLM_CMD_CREG,      "+CREG?"    },
    {WLM_CMD_CPIN,      "+CPIN?"    },
    {WLM_CMD_CPIN0,     "+CPIN="    },
    {WLM_CMD_CGATT0,    "+CGATT=0"  },
    {WLM_CMD_CGATT1,    "+CGATT=1"  },
    {WLM_CMD_DIAL,      "D*99***1#" },
    {WLM_CMD_D2,        "&D2"},
    {WLM_CMD_COPS,      "+COPS?"    },
    {WLM_CMD_CGMR,      "+CGMR"    },
    {WLM_CMD_CGSN,      "+CGSN"      },
    {WLM_CMD_CCID,      "+CCID"     },
    {WLM_CMD_END,       ""          },
    {WLM_CMD_UNDEFINE,  NULL        }
};

const WLMCMDRESPTABLE SIM800C_RET_List[]= {
    {WLM_CMD_UNDEFINE, NULL}
};


const WLMATCMDMAP MC8618_CMD_List[]= {
    {WLM_CMD_E0,        "E0"        },
    {WLM_CMD_CSQ,       "+CSQ"      },
    {WLM_CMD_CREG,      "+CREG?"    },
    {WLM_CMD_CPIN,      "+CPIN?"    },
    {WLM_CMD_CPIN0,     "+CPIN=1,"  },
    {WLM_CMD_CGATT0,    ""          },
    {WLM_CMD_CGATT1,    ""          },
    {WLM_CMD_DIAL,      "D#777"     },
    {WLM_CMD_D2,        "&D2"       },
    {WLM_CMD_COPS,      ""  },
    {WLM_CMD_CGMR,      "+GMR"      },
    {WLM_CMD_CGSN,      "+ZMEID"      },
    {WLM_CMD_CCID,          "+GETICCID"},
    {WLM_CMD_END,       ""          },
    {WLM_CMD_UNDEFINE,  NULL        }
};
const WLMRESPTABLE MC8618_RET_List[] = {
    {WLM_CMD_UNDEFINE,NULL,NULL}
};


const WLMATDrv wlm_at_drv[]= {
    {WLM_GPRS_MC39I, (WLMATCMDMAP *)&MC39i_CMD_List, (WLMCMDRESPTABLE *)&MC39I_RET_List},
    {WLM_GPRS_SM300, (WLMATCMDMAP *)&SM300_CMD_List, (WLMCMDRESPTABLE *)&SM300_RET_List},
    {WLM_CDMA_DTGS800, (WLMATCMDMAP *)&DTGS800X_CMD_List, (WLMCMDRESPTABLE *)&DTGS800X_RET_List},
    {WLM_CDMA_DTM228C, (WLMATCMDMAP *)&DTM228C_CMD_List, (WLMCMDRESPTABLE *)&DTM228C_RET_List},
    {WLM_CDMA_CE910, (WLMATCMDMAP *)&CE910_CMD_List, (WLMCMDRESPTABLE *)&CE910_RET_List},
    {WLM_CDMA_MC8331, (WLMATCMDMAP *)&MC8331_CMD_List, (WLMCMDRESPTABLE *)&MC8331_RET_List},
    {WLM_CDMA_EM200, (WLMATCMDMAP *)&EM200_CMD_List, (WLMCMDRESPTABLE *)&EM200_RET_List},
    {WLM_CDMA_MC8618, (WLMATCMDMAP *)&MC8618_CMD_List, (WLMCMDRESPTABLE *)&MC8618_RET_List},
    {WLM_GPRS_M72, (WLMATCMDMAP *)&M72_CMD_List, (WLMCMDRESPTABLE *)&M72_RET_List},
    {WLM_GPRS_BGS2, (WLMATCMDMAP *)&BGS2_CMD_List, (WLMCMDRESPTABLE *)&BGS2_RET_List},
    {WLM_GPRS_G610, (WLMATCMDMAP *)&G610_CMD_List, (WLMCMDRESPTABLE *)&G610_RET_List},
    {WLM_GPRS_QW200, (WLMATCMDMAP *)&QW200_CMD_List, (WLMCMDRESPTABLE *)&QW200_RET_List},
    {WLM_GPRS_SIM800C, (WLMATCMDMAP *)&SIM800C_CMD_List, (WLMCMDRESPTABLE *)&SIM800C_RET_List},
    {WLM_WCDMA_H350, (WLMATCMDMAP *)&H350_CMD_List, (WLMCMDRESPTABLE *)&H350_RET_List},
    {WLM_WCDMA_EHS5_USB, (WLMATCMDMAP *)&EHS5_CMD_List, (WLMCMDRESPTABLE *)&EHS5_RET_List},
    {WLM_WCDMA_EHS5_SERIAL, (WLMATCMDMAP *)&EHS5_CMD_List, (WLMCMDRESPTABLE *)&EHS5_RET_List},
	{WLM_EVDO_DE910, (WLMATCMDMAP *)&DE910_CMD_List, (WLMCMDRESPTABLE *)&DE910_RET_List},
	{WLM_EVDO_DE910_USB, (WLMATCMDMAP *)&DE910_CMD_List, (WLMCMDRESPTABLE *)&DE910_RET_List},
    {-1, NULL, NULL}
};

#endif // WLM_CMDLIST_H_INCLUDED
