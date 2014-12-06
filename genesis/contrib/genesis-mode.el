;;; genesis-mode.el -- Major mode for editing GENESIS neuron simulator script files.
;;; Modified from: wpdl-mode-el and from DerivedMode example in Emacs Wiki.

;;; Installation:

;; Load this file in (X)Emacs. Placing it under ~/.emacs.d/ should
;; make it autoload it Emacs. Otherwise place the following lines in
;; your ~/.emacs file:
;; 	(add-to-list 'load-path "~/.emacs.d/")
;; 	(require 'genesis-mode)
;; In XEmacs, you could place the following in your ~/.xemacs/init.el file:
;; 	(load-file "~/.xemacs/genesis-mode.el")

;;; Functionality:
;; 
;; This mode provides:
;; * syntax highlighting for Genesis keywords, functions, and objects. 
;; * automatic indentation with the tab key
;; * function index menu item
;;
;; This mode uses an example used in a tutorial about Emacs
;; mode creation. The tutorial can be found here:
;; http://renormalist.net/Renormalist/EmacsLanguageModeCreationTutorial

;; Authors: Cengiz Gunay <cengique@users.sf.net> and 
;; 	    Hugo Cornelis <hugo.cornelis@gmail.com>
;; WPDL-Mode Author: Scott Andrew Borton <scott@pp.htv.fi>

;; Contributors:
;; * Jim Perlewitz <perlewitz@earthlink.net> - Supplied full list of
;; 	Genesis 2.3 functions and objects (2014/06/06).
;; * Zbigniew Jędrzejewski-Szmek <zjedrzej@gmu.edu> - Bugfixes and
;; 	testing (2014/06/05).

;; Created: 30 May 2014
;; Keywords: Genesis major-mode

;; Copyright (C) 2014 Cengiz Gunay <cengique@users.sf.net>
;; Copyright (C) 2005-2006 Hugo Cornelis
;; Copyright (C) 2000, 2003 Scott Andrew Borton <scott@pp.htv.fi>

;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License as
;; published by the Free Software Foundation; either version 2 of
;; the License, or (at your option) any later version.

;; This program is distributed in the hope that it will be
;; useful, but WITHOUT ANY WARRANTY; without even the implied
;; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
;; PURPOSE.  See the GNU General Public License for more details.

;; You should have received a copy of the GNU General Public
;; License along with this program; if not, write to the Free
;; Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
;; MA 02111-1307 USA

;; TODO:
;; - function menu doesn't work in Xemacs
;; - Unify syntax highlighting for both emacses
;; - Indentation bugs: 
;;   - 'end' on line after continuation ends are ignored
;;   - Sometimes takes previous line's wrong indentation even if it's
;;   only whitespace
;;   - if two continuation blocks don't have any indentation hints in
;;   between (like an end keyword), they fuse together onto same
;;   offset.

(require 'custom)
(require 'cc-vars)
(require 'cc-mode)
(require 'cc-menus)

;;; Code:
(defvar genesis-mode-hook nil)
(defvar genesis-mode-map
  (let ((genesis-mode-map (make-keymap)))
    (define-key genesis-mode-map "\C-j" 'newline-and-indent)
    genesis-mode-map)
  "Keymap for GENESIS major mode")

(defvar genesis-mode-syntax-table
  (let ((genesis-mode-syntax-table (make-syntax-table)))
    ;; Comment styles are same as C++
    (modify-syntax-entry ?/ ". 124b" genesis-mode-syntax-table)
    (modify-syntax-entry ?* ". 23" genesis-mode-syntax-table)
    (modify-syntax-entry ?\n "> b" genesis-mode-syntax-table)
    genesis-mode-syntax-table)
  "Syntax table for genesis-mode")

;; recognise .g files as genesis files
(add-to-list 'auto-mode-alist '("\\.g\\'" . genesis-mode))

;; recognise .p files as c++ files
(add-to-list 'auto-mode-alist '("\\.p$" . c++-mode))


(defvar genesis-keywords-functions
  '(;; genesis keywords missing from below
    "function" "continue" "break" "else" "elif"
    ;; genesis2.3 command list from Jim Perlewitz
    "abort" "abs" "acos" "addaction" "addalias" "addclass" "addescape"
    "addfield" "addforwmsg" "addglobal" "addmsg" "addmsgdef"
    "addobject" "addtask" "argc" "arglist" "argv" "asciidata" "asin"
    "atan" "async" "barrier" "barrierall" "calcCm" "calcRm" "call"
    "callfunc" "cd" "ce" "cellsheet" "check" "chr" "clearbuffer"
    "clearerrors" "clearthread" "clearthreads" "closefile" "convert"
    "copy" "cos" "countchar" "countelementlist" "cpu" "create"
    "createmap" "dd3dmsg" "debug" "delete" "deleteaction" "deleteall"
    "deleteclass" "deletefield" "deleteforwmsg" "deletejob"
    "deletemsg" "deletemsgdef" "deletetasks" "disable"
    "duplicatetable" "echo" "egg" "el" "enable" "end" "enddump" "eof"
    "error" "exists" "exit" "exp" "file2tab" "fileconnect" "findchar"
    "findsolvefield" "floatformat" "flushfile" "for" "foreach"
    "gaussian" "gctrace" "gen2spk" "gen3dmsg" "genesis" "getarg"
    "getclock" "getdate" "getdefault" "getelementlist" "getenv"
    "getfield" "getfieldnames" "getglobal" "getinput" "getmsg"
    "getparamGA" "getpath" "getsolvechildname" "getsolvecompname"
    "getstat" "getsyncount" "getsyndest" "getsynindex" "getsynsrc"
    "gftrace" "h" "help" "if" "include" "initdump"
    "initparamBF" "initparamCG" "initparamGA" "initparamSA"
    "initparamSS" "input" "isa" "le" "listactions" "listcommands"
    "listescape" "listfiles" "listglobals" "listobjects" "loadtab"
    "log" "logfile" "max" "maxerrors" "maxwarnings" "min" "move"
    "msgsubstitute" "mynode" "mypvmid" "mytotalnode" "myzone" "nnodes"
    "normalizeweights" "notes" "npvmcpu" "ntotalnodes" "nzones"
    "objsubstitute" "openfile" "paroff" "paron" "pastechannel"
    "pixflags" "planarconnect" "planardelay" "planardelay2"
    "planarweight" "planarweight2" "plane" "pope" "position" "pow"
    "printargs" "printenv" "pushe" "putevent" "pwe" "quit" "raddmsg"
    "rallcalcRm" "rand" "randcomp" "randcoord" "randseed" "readcell"
    "readfile" "readsolve" "reclaim" "relposition" "resched" "reset"
    "resetfastmsg" "resetsynchanbuffers" "restore" "return" "rmsmatch"
    "rotcoord" "round" "rshowmsg" "rvolumeconnect" "rvolumedelay"
    "rvolumeweight" "save" "scaleparms" "scaletabchan" "setclock"
    "setdefault" "setenv" "setfield" "setfieldprot" "setglobal"
    "setmethod" "setparamGA" "setpostscript" "setpriority" "setprompt"
    "setrand" "setrandfield" "setsearch" "setspatialfield"
    "setupalpha" "setupgate" "setupghk" "setupNaCa" "setuptau" "sh"
    "shapematch" "showclocks" "showcommand" "showfield" "showjobs"
    "showmsg" "showobject" "showsched" "showstat" "silent" "simdump"
    "simobjdump" "simundump" "sin" "spkcmp" "sqrt" "stack" "step"
    "stop" "strcat" "strcmp" "strlen" "strncmp" "strsub"
    "substituteinfo" "substring" "swapdump" "syndelay" "tab2file"
    "tan" "threadsoff" "threadson" "trunc" "tweakalpha" "tweaktau"
    "upload" "useclock" "version" "volumeconnect" "volumedelay"
    "volumedelay2" "volumeweight" "volumeweight2" "waiton" "warning"
    "where" "while" "writecell" "writefile" "xcolorscale"
    "xflushevents" "xgetstat" "xhide" "xinit" "xmap" "xpixflags" "xps"
    "xshow" "xshowontop" "xsimplot" "xtextload")
    "Keyword and function highlighting expressions for GENESIS mode.")

(defvar genesis-types
    '("str" "int" "float")
    "Data type highlighting list for GENESIS mode.")

(defvar genesis-objects
  '(;; genesis2.3 object list from Jim Perlewitz
    "asc_file" "autocorr" "axon" "Ca_concen" "calculator" "channelC2"
    "channelC3" "compartment" "concchan" "concpool" "crosscorr"
    "ddsyn" "difbuffer" "dif2buffer" "diffamp" "difshell" "disk_out"
    "disk_in" "diskio" "efield" "enz" "event_tofile" "facsynchan"
    "fixbuffer" "ferq_monitor" "funcgen" "fura2" "ghk" "hebbsynchan"
    "hh_channel" "hillpump" "hsolve" "interspike" "izcell" "Kpores"
    "ksolve" "leakage" "metadata" "Mg_block" "mmpump" "Napores"
    "nernst" "neutral" "par_asc_file" "par_disk_out" "paramtableBF"
    "paramtableCG" "paramtableGA" "paramtableSA" "paramtableSS"
    "peristim" "PID" "playback" "pool" "postmaster" "pulsegen"
    "randomspike" "RC" "reac" "receptor" "receptor2" "res_asc_file"
    "script_out" "sigmoid" "spikegen" "spikehistory" "symcompartment"
    "synchan" "synchan2" "SynE_object" "SynG_object" "SynS_object"
    "tab2Dchannel" "tabchannel" "tabcurrent" "tabgate" "table"
    "table2D" "taupump" "timetable" "variable" "vdep_channel"
    "vdep_gate" "xaxis" "xbutton" "xcell" "xcoredraw" "xdialog"
    "xdraw" "xdumbdraw" "xfastplot" "xform" "xgif" "xgraph" "ximage"
    "xlabel" "xpix" "xplot" "xshape" "xsphere" "xtext" "xtoggle"
    "xtree" "xvar" "xview")
    "Object highlighting expressions for GENESIS mode.")

;; Modified from DerivedMode example
(if (featurep 'emacs)
    (defvar genesis-font-lock-defaults
      `((
	 ;; stuff between "
	 ("\"\\.\\*\\?" . font-lock-string-face)
	 ;; ; : , ; { } =>  @ $ = are all special elements
	 (":\\|,\\|;\\|{\\|}\\|=>\\|@\\|$\\|=" . font-lock-keyword-face)
	 ( ,(regexp-opt genesis-keywords-functions 'words) . font-lock-builtin-face)
	 ( ,(regexp-opt genesis-types 'words) . font-lock-type-face) 
	 ( ,(regexp-opt genesis-objects 'words) . font-lock-constant-face) 
	 ("^[ \n\t]*\\(function\\|extern\\)[ \t]+\\([^ \t(]+\\)" 2 font-lock-function-name-face)
	 )))
  (defvar genesis-font-lock-defaults
    `(
     ;; stuff between "
     ("\"\\.\\*\\?" . font-lock-string-face)
     ;; ; : , ; { } =>  @ $ = are all special elements
     (",\\|;\\|=" . font-lock-keyword-face)
     ( ,(regexp-opt genesis-keywords-functions 'words) . font-lock-builtin-face)
     ( ,(regexp-opt genesis-types 'words) . font-lock-type-face) 
     ( ,(regexp-opt genesis-objects 'words) . font-lock-constant-face) 
     ("^[ \n\t]*\\(function\\|extern\\)[ \t]+\\([^ \t(]+\\)" 2 font-lock-function-name-face)
     ("\\(--- .* ---\\|=== .* ===\\)" . font-lock-string-face)
     )))


(defvar genesis-tab-width 2 "Width of a tab for GENESIS mode")

;; CG: This is the main function I needed from WPDL-Mode. It's been
;; heavily modified for GENESIS syntax.
(defun genesis-indent-line ()
  "Indent current line as GENESIS code."
  (interactive)
  (beginning-of-line)
  (setq check-is-cont #'(lambda () (looking-at "^.*\\\\[ \t]*\\($\\|//\\)"))) ; line ends with '\'?
  (if (bobp)
      (indent-line-to 0)	   ; First line is always non-indented
    (let ((not-indented t) cur-indent 
	  ;; TODO: make new variable that tracks a cont-block during reverse parsing
	  (is-cont (or (funcall check-is-cont) 	; either this or 
		       (save-excursion 
			 (forward-line -1) ; previous line
			 (funcall check-is-cont))))) ; is a continuation
      ;; decrease indentation if..
      (if (or (looking-at "^[ \t]*\\(end\\|pope\\|else\\|elif\\)") ; If the line we are looking at is the end of a block
	      ;; or if a line-continuation block just ended
	      (and 
	       (not is-cont)		; We're not in continuation
	       (save-excursion 		; A continuation block just ended
		 (and
		  (forward-line -1)	; Previous line is not cont,
		  (not (funcall check-is-cont))
		       (forward-line -1)	; but one before that was.
		  (funcall check-is-cont)))))
	  (progn
	    (save-excursion
	      (forward-line -1)
	      (setq cur-indent (- (current-indentation) genesis-tab-width)))
	    (if (< cur-indent 0) ; We can't indent past the left margin
		(setq cur-indent 0)))
	(save-excursion
	  (while not-indented ; Iterate backwards until we find an indentation hint
	    (forward-line -1)
	    ;; indent at the same level if..
	    ;; This hint indicates that we need to align with the end token
	    (if (looking-at "^[ \t]*\\(end\\|pope\\)")
		(progn
		  (setq cur-indent (current-indentation))
		  (setq not-indented nil))
	      ;; This hint indicates that we need to indent an extra level
	      (if (or (looking-at "^[ \t]*\\(if\\|else\\|elif\\|function\\|foreach\\|for\\|pushe\\)")
		      ;; a new line-continuation block started
		      (and is-cont	; We were in continuation
			   (funcall check-is-cont) ; and we are still in.
			   (save-excursion 
			     (forward-line -1)	; But previous line is not cont.
			     (not (funcall check-is-cont))))) ; So this is the first line of cont.
		  (progn
		    (setq cur-indent (+ (current-indentation) genesis-tab-width)) ; Do the actual indenting
		    (setq not-indented nil))
		(if (bobp)
		    (setq not-indented nil)))))))
      (if cur-indent
	  (indent-line-to cur-indent)
	(indent-line-to 0))))) ; If we didn't see an indentation hint, then allow no indentation

;;; configure function menu for genesis mode
;;; Hugo Cornelis <hugo.cornelis@gmail.com>
(if (featurep 'xemacs)
    (defvar fume-function-name-regexp-genesis ; For Xemacs, use func-menu
      "^[ \t]*function\\([ \t]+\\([_a-zA-Z][_a-zA-Z0-9]*\\)\\|[ \t]+\\)"
      "Expression to get genesis function Names")
  (setq fume-function-name-regexp-genesis ; For GNU Emacs, use Imenu
    '(("Function" "^function\\([ \t]+\\([_a-zA-Z][_a-zA-Z0-9]*\\)\\|[ \t]+\\)" 1))
    ))


;;; Specialised routine to get the next genesis function in the buffer
;;; Hugo Cornelis <hugo@bbf.uia.ac.be>
(defun fume-find-next-genesis-function-name (buffer)
  "Searches for the next genesis function in BUFFER."
  (set-buffer buffer)
  ;; Search for the function
  (if (re-search-forward fume-function-name-regexp-genesis nil t)
      (let ((char (progn
                    (backward-up-list 1)
                    (save-excursion
                      (goto-char (scan-sexps (point) 1))
                      (skip-chars-forward "[ \t\n]")
                      (following-char)))))
        ;; Skip this function name if it is a prototype declaration.
	;; should still be recoded for genesis function prototypes.
	;; then I have to test on '^[ \t]*extern' ?
        (if (eq char ?\;)
            (fume-find-next-c-function-name buffer)
          (let (beg
                name)
            ;; Get the function name and position
            (forward-sexp -1)
            (setq beg (point))
            (forward-sexp)
            (setq name (buffer-substring beg (point)))
            ;; ghastly crock for DEFUN declarations
            (cond ((string-match "^DEFUN\\s-*" name)
                   (forward-word 1)
                   (forward-word -1)
                   (setq beg (point))
                   (cond ((re-search-forward "\"," nil t)
                          (re-search-backward "\"," nil t)
                          (setq name
                                (format "%s %s"
                                        name
                                        (buffer-substring beg (point))))))))
            ;; kludge to avoid `void' etc in menu
            (if (string-match
                "\\`\\(void\\|if\\|else if\\|else\\|switch\\|for\\|while\\)\\'"
		name)
                (fume-find-next-c-function-name buffer)
              (cons name beg)))))))

;; Register function menu rules for Xemacs
;; add function parser for genesis mode
(if (featurep 'xemacs)
    (progn 
      (require 'func-menu)
      (setq fume-function-name-regexp-alist
	    (append '((genesis-mode . fume-function-name-regexp-genesis))
		    fume-function-name-regexp-alist))
      (setq fume-find-function-name-method-alist
	    (append '((genesis-mode . fume-find-next-genesis-function-name))
		    fume-find-function-name-method-alist))))

(add-hook 'genesis-mode-hook
	  (lambda ()
	    (if (featurep 'xemacs)
		(progn
		  (turn-on-fume-mode)
		  (easy-menu-define c-genesis-menu genesis-mode-map "Genesis Mode Commands"
		    (c-mode-menu "Genesis"))
		  (easy-menu-add c-genesis-menu genesis-mode-map))
	      (progn			; In GNU Emacs
		(setq imenu-generic-expression fume-function-name-regexp-genesis)
		(imenu-add-menubar-index)))))



;; CG: Use DerivedMode from c-mode to take comment definitions
(define-derived-mode genesis-mode fundamental-mode "GENESIS script"
    "GENESIS major mode is for editing GENESIS neuron simulator script files.

The hook variable `genesis-mode-hook' is run with no args, if that
variable is bound and has a non-nil value.

Key bindings:
\\{genesis-mode-map}"

    ;; From Hugo
    (interactive)
    ;(c-initialize-cc-mode)

    ;; from WPDL mode example
    (use-local-map genesis-mode-map)  
  
    ;; set fontification rules for (X)Emacs
    (if (featurep 'emacs)
	(setq font-lock-defaults genesis-font-lock-defaults)
      (setq font-lock-defaults '(genesis-font-lock-defaults)))
      
    ;; when there's an override, use it
    ;; otherwise it gets the default value
    (when genesis-tab-width
      (setq tab-width genesis-tab-width))

    ;; indentation
    (set (make-local-variable 'indent-line-function) 'genesis-indent-line)  

    ;; comment highlighting
    (setq comment-start "/*")
    (setq comment-end "*/")

    (set-syntax-table genesis-mode-syntax-table)
    
    ;; From Hugo (these override Genesis indentation)
    ;(c-common-init)

    ;; Call mode hooks
    ;(run-hooks 'c-mode-common-hook)
    (run-hooks 'genesis-mode-hook)
    ;(c-update-modeline)
    )

(provide 'genesis-mode)

;;; genesis-mode.el ends here



