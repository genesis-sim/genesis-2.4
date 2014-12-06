/*
** $Id: sim_func_ext.h,v 1.2 2005/06/27 19:01:07 svitak Exp $
** $Log: sim_func_ext.h,v $
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.16  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.15  2000/03/27 10:34:44  mhucka
** Commented out statusline functionality, because it caused GENESIS to
** print garbage control characters upon exit, screwing up the user's
** terminal.  Also added return type declaractions for various things.
**
** Revision 1.14  1999/08/22 04:42:15  mhucka
** Various fixes, mostly for Red Hat Linux 6.0
**
** Revision 1.13  1997/07/23 21:52:47  venkat
** Added extern declaration of quitCallback()
**
** Revision 1.12  1997/05/23 01:40:49  dhb
** Added DeleteMsgReferringToData() which deletes outbound messages
** with data slots pointing to data which is no longer valid.
**
** Revision 1.11  1997/05/23 00:28:40  dhb
** Added decl for RemapMsgData().
**
** Revision 1.10  1996/05/16 21:46:31  dhb
** Change messages back to a linked structure.
**
** MsgIn and MsgOut are now unified into a single Msg structure which
** is linked on two lists (inbound and outbound).
**
** GetMsgInByMsgOut() and GetMsgOutByMsgIn() are now macros which just
** return the Msg structure.
**
** The MsgIn and MsgOut types are still supported but are now aliases
** for Msg.
**
 * Revision 1.9  1994/08/31  02:57:39  dhb
 * Added FieldNameProper().
 *
 * Revision 1.8  1994/05/27  22:26:58  dhb
 * Added all the fieldlist interface functions.
 *
 * Revision 1.7  1993/12/23  03:00:58  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.6  1993/06/29  18:56:07  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.5  1993/04/23  22:11:48  dhb
 * Added various GetMsg...By...() functions
 *
 * Revision 1.4  1993/04/23  21:52:36  dhb
 * Added extern statements for additional extended field functions.
 *
 * Revision 1.3  1993/03/10  23:15:58  dhb
 * Extended element fields.
 *
 * Revision 1.2  1992/12/18  21:19:06  dhb
 * Made AddMsgIn and AddMsgOut extern (defined in sim_msg.c) to
 * support compact messages
 *
 * Revision 1.1  1992/10/27  21:19:36  dhb
 * Initial revision
 *
*/

#ifndef SIM_FUNC_EXT_H
#define SIM_FUNC_EXT_H

#include <stdlib.h>
#include <string.h>
#include "sim_struct.h"
#include "shell_struct.h"

extern char				*ArgListToString();
extern FILE				*CurrentScriptFp();
extern FILE				*NextScriptFp();
extern float				GetVarNumData();
extern char				*GetVarStrData();
extern char				*array();
extern PFI				LoadFunction();
extern FuncTable			GetCommand();
extern PFI				GetFuncAddress();
extern char				*FindSrcLine();
extern char				*GetFuncName();
extern char				*date();
extern float				Atof();
extern double				Atod();
extern char				*ftoa();
extern char				*itoa();

/* 1999-07-16 <mhucka@bbb.caltech.edu>
 * This fix for Red Hat Linux 6.0 is courtesy of Mike Vanier
 * <mvanier@bbb.caltech.edu>.  
 */
#ifdef linux
extern char                             *strchr __P ((__const char *__s, int __c));
#else
extern char	                        *strchr();
#endif

extern void            ClearPinfo();
extern int             LocateOp();
extern char            *GetIdent();
extern char            *NextDelimiter();
extern char            *CloseDelimiter();
extern int             MatchWildcard();
extern char            *LocalPath();
extern void            GetPathInfo();
extern int             CheckOptions();
extern void            FillList();
extern ElementList     *SubWildcardGetElement();
extern ElementList     *WildcardGetElement();
extern void            Enable();
extern void            Block();
extern int             IsEnabled();
extern void            HsolveEnable();
extern void            HsolveBlock();
extern int             IsHsolved();
extern int             IsActive();
extern void            ActionHashInit();
extern int             ActionHashPut();
extern Action          *GetAction();
extern char            *GetActionName();
extern void            AddAction();
extern void            BasicActions();
extern int             AddActionToObject();
extern int             UpdateActionObjects();
extern int             DeleteActionFromObject();
extern int             ActionIsPermanent();
extern void            ActionListMakePermanent();
extern int             ActionListEqual();
extern char            *Actionname();
extern char            *CheckActionType();
extern int             CheckActionName();
extern ActionList*     GetActionListByType();
extern PFI             GetActionFunc();
extern int             CallScriptActionFunc();
extern int             CallActionsSubToBase();
extern int             CallActionsBaseToSub();
extern int             CallActionByActionList();
extern int             CallActionFunc();
extern int             CallEventActionWithTime();
extern int             CallEventAction();
extern void            SimExecuteFunction();
extern int             IndexOffset();
extern int             CalculateOffset();
extern int             StructSize();
extern int             ComputeMode();
extern int             ComputeIndirection();
extern char            *CalculateAddress();
extern int             Attach();
extern int             AttachToEnd();
extern void            OK();
extern void            FAILED();
extern void            SetSimFlags();
extern void            ValidElements();
extern void            DisplayProfile();
extern Element         *RootElement();
extern void            StartupElements();
extern void            SimStartup();
extern char            *GetBinding();
extern void            DeleteBuffer();
extern int             BufferCopy();
extern int             BufferCreate();
extern int             CallElement();
extern int             CallElementParentObject();
extern int             CallElementVirtualObject();
extern int             CallElementWithMode();
extern void            CheckSimulation();
extern int             CheckClass();
extern int             ClassListEqual();
extern void            ClassHashInit();
extern int             ClassHashPut();
extern int             ClassHashFind();
extern int             ClassID();
extern char            *GetClassName();
extern void            AddClass();
extern int             ClassIsPermanent();
extern void            ClassListMakePermanent();
extern void            BasicClasses();
extern void            CheckClocks();
extern void            CheckClocksSet();
extern void            ShowClocks();
extern void            AssignElementClock();
extern void            SetClock();
extern Event           *CopyEvents();
extern Element         *CopyElement();
extern Element         *CopyElementTree();
extern int             CopyAction();
extern int             CreateAction();
extern Element         *Create();
extern char            *CreateObject();
extern Element         *WorkingElement();
extern void            SetWorkingElement();
extern void            SetRecentElement();
extern Element         *RecentElement();
extern void            SetRecentConnection();
extern Connection      *RecentConnection();
extern void            FreeTree();
extern int             ElementReaper();
extern int             DeleteElement();
extern int             FreeElement();
extern int             DetachElement();
extern int             AddObjToDumpList();
extern void            DumpObj();
extern void            FillDumpList();
extern void            WriteDumpHeaders();
extern void            WriteDumpTail();
extern void            WriteDumpList();
extern int             ObjectSubstitute();
extern void            DumpInterpol();
extern int             IsDumpDisabled();
extern void            EnableDump();
extern void            DisableDump();
extern void            MessageSubstitute();
extern Element         *GetChildElement();
extern int             IsElementWithinRegion();
extern int             IsElementWithinRegionRel();
extern ElementList     *CreateElementList();
extern void            AddElementToList();
extern void            FreeElementList();
extern int             IsElementInList();
extern void            ClearBuffer();
extern void            ResetBuffer();
extern int             AddEventBuffer();
extern void            ReplaceEvent();
extern int             ExpandBuffer();
extern void            IncrementEnd();
extern int             InsertEvent();
extern int             AppendEvent();
extern int             PutEvent();
extern Event           *FindEvent();
extern Event           *PreviousEvent();
extern int             AddExtField();
extern int             DelExtField();
extern void            DelAllExtFields();
extern int             SetExtField();
extern char*           GetExtField();
extern int             CopyExtFields();
extern int             MemUsageExtFields();
extern void            DisplayExtFields();
extern void            AddExtFieldsToString();
extern double          ExtFieldMessageData();
extern double          IndFieldMessageData();
extern char*           GetExtFieldAdr();
extern void            FreeExtFieldAdr();
extern char*           GetExtFieldNameByAdr();
extern GenesisObject*  BaseObject();
extern int             ElementIsA();
extern void            ObjectFree();
extern void            ObjectCacheEnter();
extern GenesisObject*  ObjectCacheFind();
extern GenesisObject*  ObjectCacheCheck();
extern GenesisObject*  ObjectModify();
extern int             SetFieldListDebug();
extern int             GetFieldListDebug();
extern int             AddFieldList();
extern void            FieldListFree();
extern int             DelFieldList();
extern int             AddDefaultFieldList();
extern void*           GetFieldListInfo();
extern int             GetFieldListType();
extern int             SetFieldListType();
extern int             GetFieldListProt();
extern int             SetFieldListProt();
extern char*           GetFieldListDesc();
extern int             SetFieldListDesc();
extern int             FieldIsPermanent();
extern int             FieldListExists();
extern void            CopyFieldList();
extern void            DisplayFieldList();
extern int             FieldListEqual();
extern void            FieldListMakePermanent();
extern FLI*            FLIOpen();
extern void            FLIClose();
extern int             FLIValid();
extern void            FLINext();
extern int             FLIType();
extern int             FLIProt();
extern char*           FLIName();
extern void*           FLIInfo();
extern int             FLIFind();
extern int             GetDatatype();
extern void            DisplayOneField();
extern void            DisplayFields();
extern void            FieldFormat();
extern char            *FieldValue();
extern void            DisplayField();
extern char            *GetFieldStr();
extern char            *GetFieldAdr();
extern char            *FormatStruct();
extern void            CloseAllAsciiFiles();
extern Element         *FindElement();
extern Element         *GetElement();
extern char*           ElmFieldValue();
extern Element **      *CreateGrid();
extern int             CreateMap();
extern void            ElementHashInit();
extern int             ElementHashPut();
extern int             ElementHashPutTree();
extern void            ElementHashRemove();
extern void            ElementHashRemoveTree();
extern Element         *ElementHashFind();
extern void            hashelminfo();
extern int             GetElementIndex();
extern int             FindFreeIndex();
extern double          DirectIntegrate();
extern double          IntegrateMethod();
extern double          Gear();
extern double          AB2();
extern double          AB3();
extern double          AB4();
extern double          AB5();
extern double          Euler_Predictor_Corrector();
extern double          Runge_Kutta();
extern void            DisplayHeader();
extern void            init_smalloc();
extern int             push_element();
extern Element         *pop_element();
extern char            *smalloc();
extern char            *scalloc();
extern int             sfree();
extern int             sfreeall();
extern int             smalloc_used();
extern void            init_smalloc();
extern Element **      *CreateGrid();
extern int             CreateMap();
extern int             CountMarkers();
extern void            ClearMarkers();
extern void            SetIntMethods();
extern int             ValidHierarchy();
extern int             MsgListEqual();
extern MsgList         *GetMsgListByName();
extern double          DoubleMessageData();
extern double          FloatMessageData();
extern double          IntMessageData();
extern double          ShortMessageData();
extern double          StringMessageData();
extern double          VoidMessageData();
extern char            *GetStringMsgData();
extern int             GetSlotType();
extern void            DisplayMsgIn();
extern MsgList         *GetMsgListByType();
extern void            ShowMsgIn();
extern void            ShowMsgOut();
extern int             UnlinkMsgIn();
extern int             UnlinkMsgOut();
extern int             GetMsgOutNumber();
extern Msg             *GetMsgOutByNumber();
extern int             GetMsgInNumber();
extern Msg             *GetMsgInByNumber();
extern int             MsgInMemReq();
extern Msg*            AllocMsg();
extern void            FreeMsg();
extern void            RemapMsgData();
extern void            DeleteMsgReferringToData();
extern int             LinkMsgIn();
extern int             LinkMsgOut();
extern void            CopyMsgs();
extern int             DeleteAllMsgsByType();
extern void            DeleteAllMsgs();
extern int             DeleteMsg();
extern void            AssignSlotFunc();
extern int             MsgListMaxType();
extern int             MsgListAdd();
extern int             MsgListDelete();
extern void            MsgListMakePermanent();
extern Msg*            AddForwMsg();
extern void            DeleteForwMsg();
extern void            AddMsg();
extern int             GetMsgNumBySrcDest();
extern int             Name();
extern void            NameHashInit();
extern int             AddName();
extern int             FindName();
extern char            *GetHeader();
extern void            Notes();
extern void            ObjectHashInit();
extern int             ObjectHashEnter();
extern int             ObjectHashPut();
extern GenesisObject   *ObjectHashFind();
extern GenesisObject   *GetObject();
extern char            *getobjenv();
extern void            AddObject();
extern void            ObjectAddClass();
extern void            BasicObjects();
extern int             SetElementEnv();
extern int             SaveData();
extern int             WriteHeader();
extern int             CloseSave();
extern int             SaveString();
extern int             SaveSlot();
extern int             SaveMsg();
extern int             SaveElement();
extern int             SaveTree();
extern int             SaveArgList();
extern int             SaveActionList();
extern int             SaveMsgList();
extern int             SaveFieldList();
extern int             SaveObject();
extern int             SaveObjects();
extern int             SaveClasses();
extern int             SaveSimulation();
extern int             PositionElement();
extern void            quitCallback();
extern void            QuitStage1();
extern float           ran1();
extern float           ran3();
extern int             FreeRegionMask();
extern int             GetRegionDefinition();
extern void            SimReset();
extern void            ResetTmin();
extern void            ResetSimState();
extern void            ResetElements();
extern void            ErrorMessage();
extern void            ActionHeader();
extern void            InvalidAction();
extern void            InvalidPath();
extern void            InvalidField();
extern size_t          memusage();
extern void            sim_set_float_format();
extern int             CheckType2Header();
extern Schedule        *GetSchedule();
extern int             GetMaxTasks();
extern int             WorkingTasks();
extern Schedule        *GetWorkingTask();
extern void            InitSchedule();
extern void            ExecuteTasks();
extern void            ClearWorkingSchedule();
extern void            Reschedule();
extern int             Server();
extern int             ServerJob();
extern int             ServerArgv();
extern int             GoodBye();
extern int             Shutdown();
extern int             GenCmd();
extern int             ExecuteServerCmd();
extern int             ServerCommand();
extern int             ServerReply();
extern int             ServerLog();
extern int             CallSetActions();
extern char*           FieldNameProper();
extern int             SetElement();
extern int             DirectSetElement();
extern int             SetField();
extern void            ShowObject();
extern void            ShowSpecific();
extern void            ShowBasic();
extern void            ShowAll();
extern void            ShowField();
extern int             IsGElement();
extern void            Simulate();
extern void            ClearCheckMarkers();
extern ElementStack    *NewPutElementStack();
extern void            NewFreeElementStack();
extern Element         *NewTopOfStack();
extern Element         *NewNextElement();
extern Element         *NewFastNextElement();
extern void            PrintStatus();
extern void            SimStatus();
extern void            SetBreakFlag();
extern int             GetCurrentStep();
extern void            StepStatus();
extern void            SetCurrentStep();
extern void            StepSimulation();
extern void            TStepSimulation();
extern void            RemoveSimulationJob();
extern void            ActivateStep();
extern void            ReportStatus();
extern int             ParseConnection();
extern char            *GetNameAndIndex();
extern int             HasElementField();
extern char            *SpecificField();
extern char            *Pathname();
extern char            *GetParentComponent();
extern char            *GetBaseComponent();
extern char            *GetPathComponent();
extern char            *GetFieldComponent();
extern int             GetIndexComponent();
extern void            SetProjectionScale();
extern void            SetProjectionTranslate();
extern void            SetProjectionRotate();
extern int             GetTreeCount();
extern char            *GetTreeName();
extern char            *AdvancePathname();
extern void            ChangeWorkingElement();
extern void            ListElements();
extern int             CountChildren();
extern int             PutElementStack();
extern void            FreeElementStack();
extern Element         *TopOfStack();
extern Element         *NextElement();
extern Element         *FastNextElement();
extern int             ValidElement();
extern char            *GetValue();
extern int             PutValue();
extern void            do_initdump();
extern void            do_create();
extern int             do_add_msg();
extern int             check_method();
extern struct mask_type *GetMaskFromArgv();

#endif /* SIM_FUNC_EXT_H */
