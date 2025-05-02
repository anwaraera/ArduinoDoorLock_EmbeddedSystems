<?php
        // set up our connection
        $servername = 'elvisdb';
        $username = '********';
        $password = '******************';
        $databasename = '********';

        // connect to database
        $connection = mysqli_connect($servername, $username, $password, $databasename);

        if (!$connection) {
            die("Connection failed: " . mysqli_connect_error());
        }


        $sql = "SELECT * FROM ESPDoorlocks";
        $result = mysqli_query($connection, $sql);

        // run the query
        if (!mysqli_query($connection, $sql)) {
            die('Error: ' . mysqli_error($connection));
        }

        if (mysqli_num_rows($result) > 0) {
            echo "<h3>Doorlock</h3>";
            echo "<table border='1' cellpadding='8'><tr>
                    <th>Attempt</th><th>Attempt Time</th><th>Attempt Pin</th><th>Attempt Status</th>
                </tr>";
            while ($row = mysqli_fetch_assoc($result)) {
                echo "<tr>
                        <td>{$row['attempt']}</td>
                        <td>{$row['attempt_time']}</td>
                        <td>{$row['attempt_pin']}</td>
                        <td>{$row['attempt_status']}</td>
                    </tr>";
            }
            echo "</table>";
        } else {
            echo "No attempts";
        }

    mysqli_close($connection);
?>
