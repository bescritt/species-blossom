/* "Species-Blossom" - a CoreWars evolver.  Copyright (C) 2005 'Varfar' 
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 1, or (at your option) any later
 * version. 
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details. 
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA. 
 */

#include "world.hpp"
#include "warrior.hpp"
#include "benchmark.hpp"
#include "blossom_utils.hpp"
#include "insn_markov.hpp"
#include "population.hpp"
#include "reproduction.hpp"
#include "ui_gnuplot.h"

#include <iostream>
#include <cstdlib>

static void print_usage(std::ostream& out) {
	out << "Usage: blossom [--rounds N] [--cycles N]" << std::endl
	    << "  --rounds N   combats per warrior-vs-benchmark pair (default 100)" << std::endl
	    << "  --cycles N   max simulator cycles per combat       (default 80000)" << std::endl
	    << "Lower values trade statistical confidence for evolution speed." << std::endl;
}

static const char* FSH_94NOP_SCN[] = {
	"./warriors/fsh94nop0.2/scn/agonyII.rc",
	"./warriors/fsh94nop0.2/scn/cwork.rc",
	"./warriors/fsh94nop0.2/scn/hsa.rc",
	"./warriors/fsh94nop0.2/scn/mischief.rc",
	"./warriors/fsh94nop0.2/scn/mlion.rc",
	"./warriors/fsh94nop0.2/scn/oos.rc",
	"./warriors/fsh94nop0.2/scn/razor.rc",
	"./warriors/fsh94nop0.2/scn/recon2.rc",
	"./warriors/fsh94nop0.2/scn/solo3.rc",
	"./warriors/fsh94nop0.2/scn/tnames.rc",
	"./warriors/fsh94nop0.2/scn/willow.rc",
	"./warriors/fsh94nop0.2/scn/win.rc",
	0 },
	*FSH_94NOP_STN[] = {
	"./warriors/fsh94nop0.2/stn/DMan.rc",
	"./warriors/fsh94nop0.2/stn/Fuse.rc",
	"./warriors/fsh94nop0.2/stn/behemot.rc",
	"./warriors/fsh94nop0.2/stn/damage2.rc",
	"./warriors/fsh94nop0.2/stn/draken.rc",
	"./warriors/fsh94nop0.2/stn/myvamp54.rc",
	"./warriors/fsh94nop0.2/stn/pdown.rc",
	"./warriors/fsh94nop0.2/stn/preserv.rc",
	"./warriors/fsh94nop0.2/stn/rfrenzy2.rc",
	"./warriors/fsh94nop0.2/stn/torcht18.rc",
	"./warriors/fsh94nop0.2/stn/tornado4.rc",
	"./warriors/fsh94nop0.2/stn/vamp02b.rc",
	0 },
	*FSH_94NOP_PAP[] = {
	"./warriors/fsh94nop0.2/pap/SOS.rc",
	"./warriors/fsh94nop0.2/pap/Tyrant.rc",
	"./warriors/fsh94nop0.2/pap/disinc.rc",
	"./warriors/fsh94nop0.2/pap/first.rc",
	"./warriors/fsh94nop0.2/pap/fixed.rc",
	"./warriors/fsh94nop0.2/pap/npaper2.rc",
	"./warriors/fsh94nop0.2/pap/revenge.rc",
	"./warriors/fsh94nop0.2/pap/skin.rc",
	"./warriors/fsh94nop0.2/pap/stylized.rc",
	"./warriors/fsh94nop0.2/pap/venom.rc",
	"./warriors/fsh94nop0.2/pap/wuncle.rc",
	"./warriors/fsh94nop0.2/pap/yorba.rc",
	0 },
	*FSH_94NOP_CDS[] = {	
	"./warriors/fsh94nop0.2/cds/Blade.rc",
	"./warriors/fsh94nop0.2/cds/Ikarus.rc",
	"./warriors/fsh94nop0.2/cds/boys.rc",
	"./warriors/fsh94nop0.2/cds/herbal.rc",
	"./warriors/fsh94nop0.2/cds/jinx2.rc",
	"./warriors/fsh94nop0.2/cds/macabres.rc",
	"./warriors/fsh94nop0.2/cds/miniblur.rc",
	"./warriors/fsh94nop0.2/cds/myblur2.rc",
	"./warriors/fsh94nop0.2/cds/pre75.rc",
	"./warriors/fsh94nop0.2/cds/srage.rc",
	"./warriors/fsh94nop0.2/cds/stalker.rc",
	"./warriors/fsh94nop0.2/cds/zooom.rc",
	0 },
	*FSH_94NOP_CLR[] = {
	"./warriors/fsh94nop0.2/clr/D2003.rc",
	"./warriors/fsh94nop0.2/clr/Myth.rc",
	"./warriors/fsh94nop0.2/clr/cereal.rc",
	"./warriors/fsh94nop0.2/clr/cshot2.rc",
	"./warriors/fsh94nop0.2/clr/fscan.rc",
	"./warriors/fsh94nop0.2/clr/g2.rc",
	"./warriors/fsh94nop0.2/clr/geist.rc",
	"./warriors/fsh94nop0.2/clr/giants.rc",
	"./warriors/fsh94nop0.2/clr/goonie.rc",
	"./warriors/fsh94nop0.2/clr/luke.rc",
	"./warriors/fsh94nop0.2/clr/theblood.rc",
	"./warriors/fsh94nop0.2/clr/twinshot.rc",
	0 },
	*FSH_94NOP_PWI[] = {
	"./warriors/fsh94nop0.2/pwi/blowrag.rc",
	"./warriors/fsh94nop0.2/pwi/dawn2.rc",
	"./warriors/fsh94nop0.2/pwi/decoys.rc",
	"./warriors/fsh94nop0.2/pwi/defensiv.rc",
	"./warriors/fsh94nop0.2/pwi/fstorm.rc",
	"./warriors/fsh94nop0.2/pwi/lordimp.rc",
	"./warriors/fsh94nop0.2/pwi/minijedi.rc",
	"./warriors/fsh94nop0.2/pwi/netpap.rc",
	"./warriors/fsh94nop0.2/pwi/rotfcopy.rc",
	"./warriors/fsh94nop0.2/pwi/safetyin.rc",
	"./warriors/fsh94nop0.2/pwi/silking.rc",
	"./warriors/fsh94nop0.2/pwi/unheard.rc",
	0 },
	*FSH_94NOP_PWS[] = {
	"./warriors/fsh94nop0.2/pws/Metal.rc",
	"./warriors/fsh94nop0.2/pws/faction4.rc",
	"./warriors/fsh94nop0.2/pws/numb.rc",
	"./warriors/fsh94nop0.2/pws/pglum.rc",
	"./warriors/fsh94nop0.2/pws/purifier.rc",
	"./warriors/fsh94nop0.2/pws/rcheep.rc",
	"./warriors/fsh94nop0.2/pws/recovery.rc",
	"./warriors/fsh94nop0.2/pws/wpaper.rc",
	0 },
	*FSH_94NOP_SAI[] = {
	"./warriors/fsh94nop0.2/sai/976.rc",
	"./warriors/fsh94nop0.2/sai/Stormkeeper.rc",
	0,
	"./warriors/fsh94nop0.2/sai/airbag.rc",
	"./warriors/fsh94nop0.2/sai/candy2.rc",
	"./warriors/fsh94nop0.2/sai/cascade.rc",
	"./warriors/fsh94nop0.2/sai/gigolo.rc",
	"./warriors/fsh94nop0.2/sai/ironicimps.rc",
	"./warriors/fsh94nop0.2/sai/rosebud.rc",
	"./warriors/fsh94nop0.2/sai/rotp.rc",
	"./warriors/fsh94nop0.2/sai/rotp2.rc",
	0 },
	*FSH_94NOP_SBI[] = {
	"./warriors/fsh94nop0.2/sbi/blinedl.rc",
	"./warriors/fsh94nop0.2/sbi/devilish.rc",
	"./warriors/fsh94nop0.2/sbi/hhead.rc",
	"./warriors/fsh94nop0.2/sbi/olivia.rc",
	"./warriors/fsh94nop0.2/sbi/pixie.rc",
	"./warriors/fsh94nop0.2/sbi/qsilver.rc",
	"./warriors/fsh94nop0.2/sbi/revival.rc",
	"./warriors/fsh94nop0.2/sbi/sovain.rc",
	"./warriors/fsh94nop0.2/sbi/uvited.rc",
	"./warriors/fsh94nop0.2/sbi/vain.rc",
	0 };
;
	
static void populate_benchmark(CBenchmark& benchmark,const char** warriors) {
	while(*warriors) {
		benchmark.add(*warriors);
		warriors++;
	}
}

static void populate_evolved(CWorld& world,CPopulation& population,const char** warriors) {
	while(*warriors) {
		population.append(new CEvolvedWarrior(world,*warriors));
		warriors++;
	}
}

static void describe_benchmark(const CBenchmark& benchmark,std::ostream& out) {
	out << '[' << benchmark.name() << ']' << std::endl;
	out << "num_warriors=" << benchmark.size() << std::endl;
	for(unsigned i=0; i<benchmark.size(); i++) {
		out << "warrior_" << i << '=' << benchmark[i].rc_name() << std::endl;
		//benchmark[i].to_red(std::cout);
	}
}

static unsigned create_benchmark(CWorld& world,const std::string& name,const char** warriors) {
		CBenchmark* benchmark = new CBenchmark(world,name);
		populate_benchmark(*benchmark,warriors);
		return world.add_benchmark(benchmark);
}

static void describe_score(const CWorld& world,const CEvolvedWarrior& warrior,std::ostream& out) {
	if(warrior.fighting() || !warrior.fought()) {
		out << "warrior " << warrior.rc_name() << " has not got a ready score yet" << std::endl;
	} else {
		const unsigned count = world.benchmark_count();
		out << "warrior " << warrior.rc_name() << " scores [";
		for(unsigned i=0; i<count; i++) {
			const CBenchmark& benchmark = world.benchmark(i);
			const unsigned score = warrior.normalised_score(benchmark);
			BLOSSOM_ASSERT(~0 != score);
			out << score;
			if(i<(count-1))
				out << ',';
			else
				out << ']';
		}
		out << std::endl;
	}
}

static void describe_population(const CWorld& world,const CPopulation& population,std::ostream& out) {
	const unsigned count = population.size();
	for(unsigned i=0; i<count; i++) {
		describe_score(world,population.warrior(i),out);
	}
}

static void describe_population(CWorld& world,const std::string& name,const char** warriors,std::ostream& out) {
	CPopulation population(world,name);
	populate_evolved(world,population,warriors);
	do; while(world.loop());
	describe_population(world,population,out);
}

static void plot(CPopulation& population) {
	// Headless / fast mode: set NO_PLOT=1 to skip gnuplot entirely so the
	// evolver is not throttled by the plotting terminal.
	if(getenv("NO_PLOT"))
		return;
	// create the datafile
	std::string dat_filename = population.name();
	dat_filename += ".dat";
	FILE* dat = fopen(dat_filename.c_str(),"w");
	BLOSSOM_ASSERT_ALWAYS(dat);
	const unsigned count = population.size();
	for(unsigned i=0; i<count; i++) {
		const CEvolvedWarrior& warrior = population.warrior(i);
		if(!warrior.fought())
			continue;
		for(unsigned b=0; b<population.world().benchmark_count(); b++) {
			if(b)
				fputc('\t',dat);
			fprintf(dat,"%d",warrior.normalised_score(population.world().benchmark(b)));
		}
		fputc('\n',dat);
	}
	fclose(dat);
	// plot it
	plotter_t* plot = population.world().plotter();
	BLOSSOM_ASSERT_ALWAYS(plot);
	pl_emit(plot,"unset hidden3d\n"
		"set ticslevel 0.5\n"
		"set autoscale\n"
		"set parametric\n"
		"set style data points\n"
		"splot \"%s\"\n",dat_filename.c_str());
}

int main(int argc,char** argv) {
	unsigned cfg_rounds = 100;   // original hard-coded default
	unsigned cfg_cycles = 80000; // original hard-coded default
	for(int i=1; i<argc; i++) {
		std::string a = argv[i];
		if((a=="--rounds" || a=="-r") && i+1<argc) {
			cfg_rounds = (unsigned)std::atoi(argv[++i]);
		} else if((a=="--cycles" || a=="-c") && i+1<argc) {
			cfg_cycles = (unsigned)std::atoi(argv[++i]);
		} else if(a=="--help" || a=="-h") {
			print_usage(std::cout);
			return 0;
		} else {
			std::cerr << "Unknown argument: " << a << std::endl;
			print_usage(std::cerr);
			return 2;
		}
	}

	std::cout << "Species-Blossom version 0.1" << std::endl;
	
	try {
		CWorld world;
		world.set_rounds(cfg_rounds);
		world.set_cycles(cfg_cycles);
		CInsnGeneratorMarkov markov(world);
		markov.load("test/koen.markov2");
		//markov.dump(std::cout);
		world.set_plotter(pl_popen());
		create_benchmark(world,"Optimax Scanners",FSH_94NOP_SCN);
		create_benchmark(world,"Optimax Papers",FSH_94NOP_PAP);
		create_benchmark(world,"Optimax Stones",FSH_94NOP_STN);
/*		describe_population(world,"Coreclears, Oneshot's and Twoshot",FSH_94NOP_CLR,std::cout);
		describe_population(world,"Clear-directing Scanner",FSH_94NOP_CDS,std::cout);
		describe_population(world,"Paper",FSH_94NOP_PAP,std::cout);
		describe_population(world,"Paper with 3/7-point imps",FSH_94NOP_PWI,std::cout);
		describe_population(world,"Paper with seperate stone",FSH_94NOP_PWS,std::cout);
		describe_population(world,"Stones with a-imps",FSH_94NOP_SAI,std::cout);
		describe_population(world,"Stones with b-imps",FSH_94NOP_SBI,std::cout);
		describe_population(world,"Scanner",FSH_94NOP_SCN,std::cout);
		describe_population(world,"Bomber and Vampires without imps",FSH_94NOP_STN,std::cout);
*/
		CPopulation* population = new CPopulation(world,"Pond");
		populate_evolved(world,*population,FSH_94NOP_SAI);
		CEvolvedWarrior* warrior = world.reproduction().Create(population->warrior(0),population->warrior(1));
		population->append(warrior);
		warrior->to_red(std::cout);
		world.add_population(population);
		while(true) {
			while(world.loop());
			population->breed(population->size() / 3);
			plot(*population);
		}
		pl_close(world.plotter());
		return 0; //success!
	} catch(const BlossomException* be) {
		std::cerr << *be;
	}
	
	return 1; // error!
}
