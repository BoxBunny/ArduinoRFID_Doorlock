-- MySQL dump 10.13  Distrib 5.7.22, for Linux (x86_64)
--
-- Host: localhost    Database: doorlock
-- ------------------------------------------------------
-- Server version	5.7.22-0ubuntu0.16.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `day_access`
--

DROP TABLE IF EXISTS `day_access`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `day_access` (
  `entry_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(11) NOT NULL,
  `enter_log` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `staying` tinyint(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`entry_id`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `day_access`
--

LOCK TABLES `day_access` WRITE;
/*!40000 ALTER TABLE `day_access` DISABLE KEYS */;
INSERT INTO `day_access` VALUES (4,'chris','2018-05-28 03:36:23',0),(5,'chris','2018-05-28 03:36:26',0),(6,'chris','2018-05-28 03:36:27',0),(7,'chris','2018-05-28 03:36:28',0),(8,'HanJiho','2018-05-28 05:20:26',0),(9,'KimTaeyang','2018-05-28 05:23:18',0),(10,'HanJiho','2018-05-28 05:23:24',0),(11,'chris','2018-05-28 05:23:30',0),(12,'KimTaeyang','2018-05-28 05:24:59',0),(13,'HanJiho','2018-05-28 05:25:28',0),(14,'chris','2018-05-28 05:30:15',0),(15,'chris','2018-05-28 05:32:44',0),(16,'HanJiho','2018-05-28 05:33:13',0);
/*!40000 ALTER TABLE `day_access` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `entry_access`
--

DROP TABLE IF EXISTS `entry_access`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `entry_access` (
  `entry_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(11) NOT NULL,
  `enter_log` datetime DEFAULT NULL,
  `exit_log` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`entry_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1057 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `entry_access`
--

LOCK TABLES `entry_access` WRITE;
/*!40000 ALTER TABLE `entry_access` DISABLE KEYS */;
INSERT INTO `entry_access` VALUES (1003,'chirs','2018-05-01 15:00:00','2018-05-01 16:00:00'),(1004,'chirs','2018-05-02 15:00:00','2018-05-02 16:00:00'),(1005,'chirs','2018-05-03 15:00:00','2018-05-03 16:00:00'),(1006,'KimTaeyang','2018-05-04 15:00:00','2018-05-04 16:00:00'),(1007,'KimTaeyang','2018-05-05 15:00:00','2018-05-05 16:00:00'),(1008,'chris','2018-05-05 16:00:00','2018-05-05 17:00:00'),(1009,'chris','2018-05-06 13:00:00','2018-05-06 14:00:00'),(1010,'KimTaeyang','2018-05-06 14:00:00','2018-05-06 15:00:00'),(1011,'chris','2018-05-07 13:00:00','2018-05-07 14:00:00'),(1012,'KimTaeyang','2018-05-07 14:00:00','2018-05-07 14:30:00'),(1013,'test','2018-05-07 16:00:00','2018-05-07 17:00:00'),(1014,'HanJiho','2018-05-08 16:00:00','2018-05-08 17:00:00'),(1015,'HanJiho','2018-05-08 18:00:00','2018-05-08 19:00:00'),(1016,'Chris','2018-05-09 13:00:00','2018-05-09 14:00:00'),(1017,'Chris','2018-05-09 15:00:00','2018-05-09 17:00:00'),(1018,'Chris','2018-05-10 14:00:00','2018-05-10 17:00:00'),(1019,'KimTaeyang','2018-05-11 11:00:00','2018-05-11 13:00:00'),(1020,'test','2018-05-12 11:00:00','2018-05-12 13:00:00'),(1021,'HanJiho','2018-05-13 13:00:00','2018-05-13 15:00:00'),(1022,'chris','2018-05-13 14:00:00','2018-05-13 16:00:00'),(1023,'KimTaeyang','2018-05-14 12:00:00','2018-05-14 15:00:00'),(1024,'HanJiho','2018-05-14 15:00:00','2018-05-14 15:30:00'),(1025,'chris','2018-05-28 02:34:18','2018-05-28 02:34:25'),(1026,'chris','2018-05-28 02:34:35','2018-05-28 02:34:39'),(1027,'chris','2018-05-28 02:35:22','2018-05-28 02:35:24'),(1028,'chris','2018-05-28 02:35:41','2018-05-28 02:35:47'),(1029,'chris','2018-05-28 02:37:02','2018-05-28 02:37:22'),(1030,'chris','2018-05-28 02:38:37','2018-05-28 02:38:43'),(1031,'KimTaeyang','2018-05-28 02:39:04','2018-05-28 02:39:10'),(1032,'chris','2018-05-28 02:44:13','2018-05-28 02:44:57'),(1033,'chris','2018-05-28 02:45:41','2018-05-28 02:45:59'),(1034,'KimTaeyang','2018-05-28 02:46:08','2018-05-28 02:46:12'),(1035,'KimTaeyang','2018-05-28 02:47:22','2018-05-28 02:47:47'),(1036,'HanJiho','2018-05-28 02:48:42','2018-05-28 02:48:46'),(1037,'HanJiho','2018-05-28 03:26:52','2018-05-28 03:27:23'),(1038,'chris','2018-05-28 03:27:42','2018-05-28 03:27:51'),(1039,'KimTaeyang','2018-05-28 03:26:58','2018-05-28 03:27:56'),(1040,'HanJiho','2018-05-28 05:20:26','2018-05-28 05:20:49'),(1042,'HanJiho','2018-05-28 05:23:24','2018-05-28 05:23:46'),(1043,'KimTaeyang','2018-05-28 05:23:18','2018-05-28 05:23:51'),(1044,'chris','2018-05-28 03:36:23','2018-05-28 05:23:56'),(1048,'chris','2018-05-28 05:23:30','2018-05-28 05:23:56'),(1049,'KimTaeyang','2018-05-28 05:23:18','2018-05-28 05:25:07'),(1051,'HanJiho','2018-05-28 05:20:26','2018-05-28 05:25:36'),(1054,'chris','2018-05-28 05:30:15','2018-05-28 05:32:50'),(1055,'chris','2018-05-28 05:32:44','2018-05-28 05:32:50'),(1056,'HanJiho','2018-05-28 05:33:13','2018-05-28 05:33:22');
/*!40000 ALTER TABLE `entry_access` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `reg_user`
--

DROP TABLE IF EXISTS `reg_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `reg_user` (
  `reg_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL,
  `user_id` varchar(20) DEFAULT NULL,
  `user_passwd` varchar(255) DEFAULT NULL,
  `nfc` varchar(20) NOT NULL,
  `reg_date` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`reg_id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `reg_user`
--

LOCK TABLES `reg_user` WRITE;
/*!40000 ALTER TABLE `reg_user` DISABLE KEYS */;
INSERT INTO `reg_user` VALUES (1,'chris','409-01','*A4B6157319038724E3560894F7F932C8886EBFCF','12 123 12 123 ','2018-05-21 18:00:51'),(2,'HanJiho','409-02','*A4B6157319038724E3560894F7F932C8886EBFCF','23 234 23 234 ','2018-05-21 18:24:58'),(3,'KimTaeyang','409-03','*A4B6157319038724E3560894F7F932C8886EBFCF','34 345 34 345 ','2018-05-21 18:25:45'),(4,'test-1','409-04','*A4B6157319038724E3560894F7F932C8886EBFCF','93 196 36 217 ','2018-05-23 17:18:22'),(5,'test-2','409-05','*A4B6157319038724E3560894F7F932C8886EBFCF','57 67 219 181','2018-05-24 22:11:10'),(6,'test-3','409-06','*A4B6157319038724E3560894F7F932C8886EBFCF','57 1 194 22 ','2018-05-25 14:25:08'),(7,'test-4','409-07','*A4B6157319038724E3560894F7F932C8886EBFCF','0 112 130 136 ','2018-05-25 14:28:51');
/*!40000 ALTER TABLE `reg_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `test_nfc`
--

DROP TABLE IF EXISTS `test_nfc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test_nfc` (
  `nfc` varchar(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `test_nfc`
--

LOCK TABLES `test_nfc` WRITE;
/*!40000 ALTER TABLE `test_nfc` DISABLE KEYS */;
INSERT INTO `test_nfc` VALUES ('setup_ok'),('setup_ok'),('setup_ok'),('setup_ok'),('setup_ok'),('');
/*!40000 ALTER TABLE `test_nfc` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-05-28 13:23:49
