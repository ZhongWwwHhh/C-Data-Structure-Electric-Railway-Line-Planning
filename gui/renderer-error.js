const func = async () => {
    const response = await window.apiError.getError()
    document.getElementById('error').textContent = response;
}

func()