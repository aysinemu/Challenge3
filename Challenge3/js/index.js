const showHiddenPass = (loginPass, loginEye) => {
    const input = document.getElementById(loginPass),
            iconEye = document.getElementById(loginEye);

    iconEye.addEventListener('click', () => {
        if (input.type === 'password') {
            input.type = 'text';
            iconEye.classList.add('ri-eye-line');
            iconEye.classList.remove('ri-eye-off-line');
        } else {
            input.type = 'password';
            iconEye.classList.remove('ri-eye-line');
            iconEye.classList.add('ri-eye-off-line');
        }
    });
};

showHiddenPass('login_pass','login-eye');

async function login_funct(e){
    console.log("Login function triggered");
    e.preventDefault();

    const username = document.getElementById("login_username").value;
    const password = document.getElementById("login_pass").value;

    try {
        const response = await fetch("/login", {
            method: "POST",
            headers: {
                "Content-Type": "application/x-www-form-urlencoded"
            },
            body: new URLSearchParams({
                username: username,
                password: password
            })
        });

        const result = await response.json();

        if(result.success){
            console.log("Login success. Redirect");
            window.location.href = window.origin + "/run";
        } else {
            alert(result.message || "Wrong user name or password");
        }

    } catch (err) {
        console.error("Error login:", err);
        alert("Can't connect");
    }
}