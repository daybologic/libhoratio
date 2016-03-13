#!/usr/bin/perl
#Horatio's Memory Manager
#Copyright (c) 2000-2014, David Duncan Ross Palmer (M6KVM), Daybo Logic
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#    * Neither the name of the Daybo Logic nor the names of its contributors
#      may be used to endorse or promote products derived from this software
#      without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
#LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#POSSIBILITY OF SUCH DAMAGE.

# This is a tool to create a sub-directory called dos_dist/ and create
# copies of relevant files in the current directory, renamed to short
# DOS names.

use File::Copy;

# Subroutines
sub IsCorrectDir();
sub Error($);
sub DoCopy($);
sub CheckUnix();
sub AllSeds($);

# We need the name of one file, to make sure we're in the correct
# directory!
my $dirvfn = 'COPYING'; # Dir verification filename.
my @all_files = ();

print "Creating DOS distribution\n";

if ( opendir(CURRENT, '.') ) {
	@all_files = readdir(CURRENT);
	closedir(CURRENT);
	if ( IsCorrectDir() ) {
		if ( mkdir('dos_dist') || -d 'dos_dist' ) {
			DoCopy('dos_dist');
		} else {
			Error ("Cannot create dos_dist/ subdirectory");
		}
	} else {
		Error "\"$dirvfn\" cannot be found, are you in the correct directory?";
	}
} else {
	die $!;
}

exit 0;

sub IsCorrectDir() {
	foreach ( @all_files ) {
		if ( $_ ) {
			return 1 if $_ eq $dirvfn
		}
	}
	return 0;
}

sub Error($) {
	my $msg = $_[0];
	printf "Error: %s\n", $msg;
}

sub DoCopy($) {
	my $dir = $_[0];

	foreach ( @all_files ) {
		my $current = $_;
		my $new; # New filename only
		my $newloc; # New filename _and_ location.
		my $uscore;

		# Filter out files which aren't essential
		if ( ($current ne 'COPYING') && ($current ne 'README')) {
			next if (
			    !( $current =~ m/\.c$/ ) and
			    !( $current =~ m/\.h$/ ) and
			    !( $current =~ m/\.cpp/ )
			);
		}

		$uscore = rindex($current, '_');
		if ( $uscore == -1 ) {
			# No underscore, copy verbatim
			$new = $current;
		} else {
			$new = substr($current, $uscore+1, (length($current)-($uscore+1)));
		}
		$newloc = $dir . '/' . $new;

		copy ($current, $newloc) || die $!;
	}
	AllSeds($dir);
}

sub AllSeds($) {
	my $dir = $_[0];

	print "Changing filenames within files (this may take some time)\n";
	if ( chdir($dir) ) {
		my @all_new_files;
		if ( opendir(DOSDIST, '.') ) {
			@all_new_files = readdir(DOSDIST);
			closedir(DOSDIST);
		}
		foreach ( @all_new_files ) {
			my $curr_new_fn = $_;

			if ( $curr_new_fn ne '.' && $curr_new_fn ne '..' ) {
				print "Processing \"$curr_new_fn\"...\n";
				foreach ( @all_files ) { # All original filenames
					my $uscore;
					my $current;
					my $new;
					my $line;

					$current = $_;
					next if ( ($current eq '.') || ($current eq '..') );

					$uscore = rindex($current, '_');
					if ( $uscore == -1 ) {
						# No underscore, use verbatim
						$new = $current;
					} else {
						$new = substr($current, $uscore+1, (length($current)-($uscore+1)));
					}

					rename $curr_new_fn, 'work';
					open(WORKFILE, '< work');
					open(OUTFILE, "> $curr_new_fn") || die $!;

					while ( $line = <WORKFILE> ) {
						$line =~ s/$current/$new/g;
						print OUTFILE $line;
					}
					close(WORKFILE);
					close(OUTFILE);
					unlink 'work';
				}
			}
		}

		chdir '..';
	}
}
