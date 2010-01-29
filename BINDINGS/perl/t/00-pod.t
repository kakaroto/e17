use Test::More;

unless ($ENV{TEST_MAINTAINER}) {
    plan skip_all => 'Set TEST_MAINTAINER to run this test';
    exit 0;
}


eval "use Test::Pod 1.00";
plan skip_all => "Test::Pod 1.00 required for testing POD" if $@;
all_pod_files_ok();
