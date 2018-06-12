
SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


--
-- Base de données :  `terranodemcu`
--

-- --------------------------------------------------------

--
-- Structure de la table `ephem`
--

CREATE TABLE `ephem` (
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `Hjour` int(11) DEFAULT NULL,
  `Mjour` int(11) DEFAULT NULL,
  `Hnuit` int(11) DEFAULT NULL,
  `Mnuit` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `temphumi`
--

CREATE TABLE `temphumi` (
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `TempC` double DEFAULT NULL,
  `HumiC` double DEFAULT NULL,
  `TempF` double DEFAULT NULL,
  `HumiF` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

