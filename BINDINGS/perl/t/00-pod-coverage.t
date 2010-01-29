use Test::More;

unless ($ENV{TEST_MAINTAINER}) {
    plan skip_all => 'Set TEST_MAINTAINER to run this test';
    exit 0;
}

eval "use Test::Pod::Coverage 1.00";
plan skip_all => "Test::Pod::Coverage 1.00 required for testing POD coverage" if $@;
all_pod_coverage_ok();
