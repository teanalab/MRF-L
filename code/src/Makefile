## your application name here
APP=umlsLceIndriRunQuery
SRC=$(APP).cpp
SRC2=
SRC3=CoordinateAscent.cpp
SRC4=QueryThread.cpp
SRC5=ConceptSelector.cpp
SRC6=ExtResourceProcessor.cpp
SRC8=QueryReformulator.cpp
SRC12=UnigramsOrigConceptSelector.cpp
SRC14=UmlsOrigMetaMapConceptSelector.cpp
SRC16=Utility.cpp
SRC19=ConceptSelectorFuns.cpp
#SOURCE=./
## extra object files for your app here
OBJ=

#prefix = /../../../../../../../ir/indri-5.8/setup
prefix = /home/fj9124/ir/indri-5.8/setup
#prefix1 = /home/fj9124/ir/projects/cpp/newIndriRunQuery/medicalExpansion/modTrec_eval9.0
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
includedir= ${prefix}/include
#includedir1= ${prefix1}/include
includedir2= ../include
objdir1=${prefix1}/obj/Debug/src
#INCPATH=-I$(includedir)
#INCPATH1=-I$(includedir1)
INCPATH=-isystem$(includedir)
#INCPATH1=-isystem$(includedir1)
INCPATH2=-I$(includedir2)
LIBPATH=-L$(libdir)

TREC_EVAL=${objdir1}/convert_zscores.o ${objdir1}/form_prefs_counts.o ${objdir1}/form_res_rels.o ${objdir1}/form_res_rels_jg.o ${objdir1}/formats.o ${objdir1}/get_prefs.o ${objdir1}/get_qrels.o ${objdir1}/get_qrels_jg.o ${objdir1}/get_qrels_prefs.o ${objdir1}/get_trec_results.o ${objdir1}/get_zscores.o ${objdir1}/m_11pt_avg.o ${objdir1}/m_binG.o ${objdir1}/m_bpref.o ${objdir1}/m_G.o ${objdir1}/m_gm_bpref.o ${objdir1}/m_gm_map.o ${objdir1}/m_infap.o ${objdir1}/m_iprec_at_recall.o ${objdir1}/m_map.o ${objdir1}/m_map_avgjg.o ${objdir1}/m_map_cut.o ${objdir1}/m_ndcg.o ${objdir1}/m_ndcg_cut.o ${objdir1}/m_ndcg_rel.o ${objdir1}/m_num_nonrel_judged_ret.o ${objdir1}/m_num_q.o ${objdir1}/m_num_rel.o ${objdir1}/m_num_rel_ret.o ${objdir1}/m_num_ret.o ${objdir1}/m_P.o ${objdir1}/m_P_avgjg.o ${objdir1}/m_prefs_avgjg.o ${objdir1}/m_prefs_avgjg_imp.o ${objdir1}/m_prefs_avgjg_ret.o ${objdir1}/m_prefs_avgjg_Rnonrel.o ${objdir1}/m_prefs_avgjg_Rnonrel_ret.o ${objdir1}/m_prefs_num_prefs_ful.o ${objdir1}/m_prefs_num_prefs_ful_ret.o ${objdir1}/m_prefs_num_prefs_poss.o ${objdir1}/m_prefs_pair.o ${objdir1}/m_prefs_pair_imp.o ${objdir1}/m_prefs_pair_ret.o ${objdir1}/m_prefs_simp.o ${objdir1}/m_prefs_simp_imp.o ${objdir1}/m_prefs_simp_ret.o ${objdir1}/m_recall.o ${objdir1}/m_recip_rank.o ${objdir1}/m_rel_P.o ${objdir1}/m_relstring.o ${objdir1}/m_Rndcg.o ${objdir1}/m_Rprec.o ${objdir1}/m_Rprec_mult.o ${objdir1}/m_Rprec_mult_avgjg.o ${objdir1}/m_runid.o ${objdir1}/m_set_F.o ${objdir1}/m_set_map.o ${objdir1}/m_set_P.o ${objdir1}/m_set_recall.o ${objdir1}/m_set_rel_P.o ${objdir1}/m_success.o ${objdir1}/m_utility.o ${objdir1}/meas_acc.o ${objdir1}/meas_avg.o ${objdir1}/meas_init.o ${objdir1}/meas_print_final.o ${objdir1}/meas_print_single.o ${objdir1}/measures.o ${objdir1}/utility_pool.o 

CXXFLAGS=-DPACKAGE_NAME=\"Indri\" -DPACKAGE_TARNAME=\"indri\" -DPACKAGE_VERSION=\"5.8\" -DPACKAGE_STRING=\"Indri\ 5.8\" -DPACKAGE_BUGREPORT=\"project@lemurproject.org\" -DINDRI_STANDALONE=1 -DHAVE_LIBM=1 -DHAVE_LIBPTHREAD=1 -DHAVE_LIBZ=1 -DHAVE_NAMESPACES=/\*\*/ -DISNAN_IN_NAMESPACE_STD=/\*\*/ -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FSEEKO=1 -DHAVE_EXT_ATOMICITY_H=1 -DP_NEEDS_GNU_CXX_NAMESPACE=1 -DHAVE_MKSTEMP=1 -DHAVE_MKSTEMPS=1 -DNDEBUG=1 -std=c++0x -ggdb -O3  -Wall -Wextra -pedantic $(INCPATH) $(INCPATH1) $(INCPATH2)
CPPLDFLAGS  =  -lindri -lz -lpthread -lm 

#all:
#	$(CXX) $(CXXFLAGS) $(SOURCE)%.cpp -o ../bin/$(APP) $(OBJ) $(LIBPATH) $(CPPLDFLAGS)

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(SRC1) $(SRC2) $(SRC3) $(SRC4) $(SRC5) $(SRC6) $(SRC7) $(SRC8) $(SRC9) $(SRC10) $(SRC11) $(SRC12) $(SRC13) $(SRC14) $(SRC15) $(SRC16) $(SRC17) $(SRC18) $(SRC19) $(SRC20) $(SRC21) -o ../bin/$(APP) $(OBJ) $(LIBPATH) $(CPPLDFLAGS)


clean:
	rm -f $(APP)


