function login(event) {
    event.preventDefault();
    // console.log(event.target);
    const formData = new FormData(event.target);
    fetch('/auth', {
        method: 'POST',
        body: JSON.stringify({
            username: formData.get('username'),
            password: formData.get('password')
        })
    }).then((response) => {
        if (response.ok) {
            document.querySelector('.login-btn').style.display = 'none';

            const body = document.getElementById('home-content');
            if (body) {
                body.appendChild(new Control());
                body.appendChild(new Canvas());
            }

            document.getElementsByTagName('main')[0].style.display = 'block';
            document.body.classList.toggle('show-popup');
        } else {
            alert('Login failed. Please try again.');
        }
    });
}
