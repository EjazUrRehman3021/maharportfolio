import os

from flask import Flask, render_template, send_from_directory

app = Flask(__name__)
_ROOT = os.path.dirname(os.path.abspath(__file__))

# HOME PAGE
@app.route("/")
def home():
    return render_template("index.html")

# ABOUT
@app.route("/about")
def about():
    return render_template("about.html")

# SKILLS
@app.route("/skills")
def skills():
    return render_template("skills.html")

# PROJECTS MAIN PAGE
@app.route("/projects")
def projects():
    return render_template("projects.html")

# PROJECT DETAIL PAGES (DYNAMIC ROUTING)
@app.route("/projects/<project_name>")
def project_detail(project_name):
    try:
        return render_template(f"projects/{project_name}.html")
    except:
        return "<h1>Project not found</h1>"

# CONTACT
@app.route("/contact")
def contact():
    return render_template("contact.html")

# RESUME DOWNLOAD
@app.route("/resume")
def resume():
    path = os.path.join(_ROOT, "resume.pdf")
    if not os.path.isfile(path):
        return "<p>Resume file not added yet. Place <code>resume.pdf</code> in the project root.</p>", 404
    return send_from_directory(_ROOT, "resume.pdf")

if __name__ == "__main__":
    port = int(os.environ.get("PORT", "5000"))
    app.run(debug=True, host="0.0.0.0", port=port)
