import module namespace geoproj = "http://www.zorba-xquery.com/modules/geoproj";
declare namespace geoproj-param = "http://www.zorba-xquery.com/modules/geoproj-param";

geoproj:wgs84-to-omerc-gmlpos(10, 10, 1 div 1000, 
                        <geoproj-param:latlong>
                           <geoproj-param:lat>11</geoproj-param:lat>
                           <geoproj-param:long>11</geoproj-param:long>
                        </geoproj-param:latlong>)
