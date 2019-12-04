WS	[ \t\n]

%{
bool skip_to_main = false;
bool monitor_registers = false;
bool user_code_only = false;
%}

%option prefix="vaccs_cfg_"

%%

skip_to_main true				{ }
skip_to_main false			{ }
monitor_registers true				{ }
monitor_registers false			{ }
user_code_only true				{ }
user_code_only false			{ }

{WS}				{ ; }


.				{
				  fprintf(stderr, "Scanner: lexical error '%s'.\n",
				  	  Calctext);
				}
