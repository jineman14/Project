DROP TABLE IF EXISTS `user_server_config`;
CREATE TABLE IF NOT EXISTS `user_server_config` (
`asn` bigint(20) unsigned not null auto_increment,
`name` varchar(64) NOT NULL DEFAULT '',
`db_index` int(10) not null,
`reg_date` timestamp not null default current_timestamp,
primary key (`asn`)
) engine=innodb default charset=utf8mb4;