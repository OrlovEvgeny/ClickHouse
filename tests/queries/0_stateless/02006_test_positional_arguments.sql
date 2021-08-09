drop table if exists test;
create table test (col1 Int32, col2 Int32, col3 Int32) engine = Memory();
insert into test select number, number, 1 from numbers(2);
insert into test select number, number, 2 from numbers(2);
insert into test select number, number+1, 1 from numbers(2);
insert into test select number, number+1, 2 from numbers(2);
insert into test select number, number, 3 from numbers(2);
insert into test select number, number, 4 from numbers(2);
insert into test select number, number+1, 3 from numbers(2);
insert into test select number, number+1, 4 from numbers(2);
insert into test select number, number, 2 from numbers(2);
insert into test select number, number+1, 2 from numbers(2);

set enable_positional_arguments = 1;

-- { echo }
select * from test order by 1;
select * from test order by 3;

select col1, col2 from test group by col1, col2 order by col2;
select col1, col2 from test group by 1, 2 order by 2;

select col2, col3 from test group by col3, col2 order by col3;
select col2, col3 from test group by 3, 2 order by 3;

select col2 from test group by 2 order by 2;
select col2 + 100 from test group by 2 order by col2;

select * from test order by col3 limit 1 by col3;
select * from test order by 3 limit 1 by 3;
