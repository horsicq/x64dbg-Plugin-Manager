from faker import Faker

# Initialize Faker instance
fake = Faker()

# Generate HTML content
html_content = """
<!DOCTYPE html>
<html>
<head>
    <title>Employee Data</title>
</head>
<body>
    <h1>Employee Data</h1>
    <table border="2">
        <tr>
            <th>Name</th>
            <th>City</th>
            <th>Phone Number</th>
        </tr>
"""

# Generate 100 random employee records
for _ in range(100):
    name = fake.name()
    city = fake.city()
    phone_number = fake.phone_number()
    html_content += f"""
        <tr>
            <td>{name}</td>
            <td>{city}</td>
            <td>{phone_number}</td>
        </tr>
    """

# Close the HTML content
html_content += """
    </table>
</body>
</html>
"""

# Write the HTML content to a file
with open("employee_data.html", "w") as file:
    file.write(html_content)

print("HTML file 'employee_data.html' has been generated with random employee data.")
